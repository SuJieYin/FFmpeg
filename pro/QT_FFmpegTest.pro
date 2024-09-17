QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    FFmpeg.pri \
    zhongqiu.mp4

include(FFmpeg.pri)

# lib
# 导入的内容如下：
INCLUDEPATH += $$PWD/ffmpeg/include
LIBS +=-L$$PWD/ffmpeg/lib -lavcodec -lavutil -lavformat -lavdevice -lavfilter -lpostproc -lswresample -lswscale
#注：-L是指在指定路径下，$$PWD是指当前路径

