#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
// ffmpeg 由于ffmpeg是C的库，故需要extern “C”进行引入
// 即 以C的方式进行编译
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
}

#include <QDebug>
#include <QDebug>
#include <QTimer>
#include <QImage>
#include <QVBoxLayout>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void openVideo(const QString &filePath);
private Q_SLOTS:
    void showNextFrame();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QTimer timer;

    // FFmpeg相关的对象
    AVFormatContext *formatContext;   // 用于保存格式上下文
    AVCodecContext *codecContext;     // 用于保存解码上下文
    AVPacket *packet;                 // 用于保存数据包
    AVFrame *frame;                   // 用于保存帧
    SwsContext *scaleContext;         // 用于图像格式转换
};
#endif // MAINWINDOW_H
