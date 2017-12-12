#-------------------------------------------------
#
# Project created by QtCreator 2017-11-29T14:36:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TestEnv
TEMPLATE = app

CONFIG  += link_pkgconfig
PKGCONFIG += opencv
LIBS += -L$$OPENCV_SDK_DIR/lib/

SOURCES += main.cpp\
    driver/QCvCamView.cpp \
    gui/CamDlg.cpp

HEADERS  += \
    driver/QCvCamView.h \
    gui/CamDlg.h

FORMS    += \
    gui/CamDlg.ui

CONFIG(debug, debug|release) {
    DESTDIR = build/debug
    OBJECTS_DIR = build/debug/.obj
    MOC_DIR = build/debug/.moc
    RCC_DIR = build/debug/.rcc
    UI_DIR = build/debug/.ui
} else {
    DESTDIR = build/release
    OBJECTS_DIR = build/release/.obj
    MOC_DIR = build/release/.moc
    RCC_DIR = build/release/.rcc
    UI_DIR = build/release/.ui
}
