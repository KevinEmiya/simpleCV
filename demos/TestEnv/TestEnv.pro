#-------------------------------------------------
#
# Project created by QtCreator 2017-11-29T14:36:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TestEnv
TEMPLATE = app

OPENCV_SDK_DIR = /home/kevin_yang/git/simpleCV/sdk/opencv_release
PKG_CONFIG_PATH = $$OPENCV_SDK_DIR/lib/pkgconfig/
CONFIG  += link_pkgconfig
PKGCONFIG += opencv
LIBS += -L$$OPENCV_SDK_DIR/lib/

SOURCES += main.cpp\
        MainWin.cpp

HEADERS  += MainWin.h

FORMS    += MainWin.ui
