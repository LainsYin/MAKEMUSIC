#-------------------------------------------------
#
# Project created by QtCreator 2015-10-22T10:15:07
#
#-------------------------------------------------

QT       += core gui
QT       += sql
LIBS     += -L$$PWD/libvlc/libvlc_lib -llibvlc -llibvlccore
INCLUDEPATH +=$$PWD/libvlc/libvlc_include
DEPENDPATH  +=$$PWD/libvlc/libvlc_include
INCLUDEPATH +=$$PWD/libvlc/include
DEPENDPATH  +=$$PWD/libvlc/include

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MakeMusic
TEMPLATE = app
CONFIG += c++11
OBJECTS_DIR += obj
UI_DIR += forms
RCC_DIR += rcc
MOC_DIR += moc
DESTDIR += bin

SOURCES += main.cpp\
        mainwindow.cpp \
    mysqlquery.cpp \
    addsongdialog.cpp \
    tablemodel.cpp \
    yqcdelegate.cpp \
    drumguitarkey.cpp \
    libvlc/src/Audio.cpp \
    libvlc/src/Common.cpp \
    libvlc/src/ControlAudio.cpp \
    libvlc/src/ControlVideo.cpp \
    libvlc/src/Enums.cpp \
    libvlc/src/Error.cpp \
    libvlc/src/Instance.cpp \
    libvlc/src/Media.cpp \
    libvlc/src/MediaList.cpp \
    libvlc/src/MediaListPlayer.cpp \
    libvlc/src/MediaPlayer.cpp \
    libvlc/src/MetaManager.cpp \
    libvlc/src/Video.cpp \
    libvlc/src/VideoDelegate.cpp \
    libvlc/src/WidgetSeek.cpp \
    libvlc/src/WidgetVideo.cpp \
    libvlc/src/WidgetVolumeSlider.cpp



HEADERS  += mainwindow.h \
    mysqlquery.h \
    addsongdialog.h \
    tablemodel.h \
    yqcdelegate.h \
    drumguitarkey.h \
    libvlc/include/Audio.h \
    libvlc/include/Common.h \
    libvlc/include/Config.h \
    libvlc/include/ControlAudio.h \
    libvlc/include/ControlVideo.h \
    libvlc/include/Enums.h \
    libvlc/include/Error.h \
    libvlc/include/Instance.h \
    libvlc/include/Media.h \
    libvlc/include/MediaList.h \
    libvlc/include/MediaListPlayer.h \
    libvlc/include/MediaPlayer.h \
    libvlc/include/MetaManager.h \
    libvlc/include/SharedExportCore.h \
    libvlc/include/SharedExportWidgets.h \
    libvlc/include/Video.h \
    libvlc/include/VideoDelegate.h \
    libvlc/include/WidgetSeek.h \
    libvlc/include/WidgetVideo.h \
    libvlc/include/WidgetVolumeSlider.h

FORMS    += mainwindow.ui \
    addsongdialog.ui \
    drumguitarkey.ui

RESOURCES += \
    resources.qrc

