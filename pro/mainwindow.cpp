#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , formatContext(nullptr)
    , codecContext(nullptr)
    , packet(nullptr)
    , frame(nullptr)
    , scaleContext(nullptr)
{
    ui->setupUi(this);
    this->setWindowTitle(qVersion());

    qInfo() << av_version_info() << Qt::endl;
    ui->label_version->setText(av_version_info());
    ui->label_version_2->setText(QString::number(avcodec_version()));

    // av_register_all() 函数在较新的 FFmpeg 版本中已经被移除

    // 初始化FFmpeg的网络功能
    avformat_network_init();

    // 计时器，用于定时读取和显示视频帧
    connect(&timer, &QTimer::timeout, this, &MainWindow::showNextFrame);
}

MainWindow::~MainWindow()
{
    if (scaleContext) {
        sws_freeContext(scaleContext);
    }
    if (frame) {
        av_frame_free(&frame);
    }
    if (packet) {
        av_packet_free(&packet);
    }
    if (codecContext) {
        avcodec_free_context(&codecContext);
    }
    if (formatContext) {
        avformat_close_input(&formatContext);
    }
    delete ui;
}

void MainWindow::openVideo(const QString &filePath)
{
    // 打开视频文件
    if (avformat_open_input(&formatContext, filePath.toStdString().c_str(), nullptr, nullptr) < 0) {
        qDebug() << "Could not open video file!";
        return;
    }

    // 获取视频流信息
    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        qDebug() << "Could not find stream information!";
        return;
    }

    // 找到第一个视频流
    int videoStreamIndex = -1;
    for (int i = 0; i < formatContext->nb_streams; i++) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }

    if (videoStreamIndex == -1) {
        qDebug() << "Could not find a video stream!";
        return;
    }

    // 获取视频流的解码器
    AVCodecParameters *codecParameters = formatContext->streams[videoStreamIndex]->codecpar;
    const AVCodec *codec = avcodec_find_decoder(codecParameters->codec_id);
    if (!codec) {
        qDebug() << "Unsupported codec!";
        return;
    }

    // 创建解码器上下文
    codecContext = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codecContext, codecParameters);
    if (avcodec_open2(codecContext, codec, nullptr) < 0) {
        qDebug() << "Could not open codec!";
        return;
    }

    // 分配帧和数据包
    packet = av_packet_alloc();
    frame = av_frame_alloc();

    // 启动计时器，每33毫秒更新帧（约30帧每秒）
    timer.start(33);
}

void MainWindow::showNextFrame()
{
    // 读取帧
    if (av_read_frame(formatContext, packet) >= 0) {
        if (packet->stream_index == 0) { // 假设我们只处理第一个视频流
            avcodec_send_packet(codecContext, packet);
            int ret = avcodec_receive_frame(codecContext, frame);
            if (ret == 0) {
                // 处理帧，转换为QImage
                if (!scaleContext) {
                    scaleContext = sws_getContext(
                        codecContext->width, codecContext->height, codecContext->pix_fmt,
                        codecContext->width, codecContext->height, AV_PIX_FMT_RGB24,
                        0, nullptr, nullptr, nullptr);
                }

                // 创建一个用于存储RGB图像的缓冲区
                uint8_t *rgbBuffer = (uint8_t *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_RGB24, codecContext->width, codecContext->height, 1));
                AVFrame *rgbFrame = av_frame_alloc();
                av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, rgbBuffer,
                                     AV_PIX_FMT_RGB24, codecContext->width, codecContext->height, 1);

                // 进行格式转换
                sws_scale(scaleContext, frame->data, frame->linesize, 0, codecContext->height, rgbFrame->data, rgbFrame->linesize);

                // 转换为QImage并显示
                QImage image(rgbFrame->data[0], codecContext->width, codecContext->height, QImage::Format_RGB888);
                ui->label_video->setPixmap(QPixmap::fromImage(image));

                av_frame_free(&rgbFrame);
                av_free(rgbBuffer);
            }
        }
        av_packet_unref(packet);
    } else {
        // 如果读取失败，可以重置视频或停止播放
        timer.stop();
    }
}

// 打开视频
void MainWindow::on_pushButton_clicked()
{
    openVideo("/home/whois/QT_FFmpegTest/zhongqiu.mp4");
}

