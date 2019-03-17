#-------------------------------------------------
#
# Project created by QtCreator 2017-12-26T22:28:01
#
#-------------------------------------------------

include(../QCvUtils/BuildSchema.pri)
include(../QCvUtils/QCvUtils.pri)

INCLUDEPATH += $$PWD/src

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FaceDetectionDemo
TEMPLATE = app

SOURCES += \
        src/main.cpp \
        src/gui/FaceVideoDlg.cpp \
        src/filter/QCvFaceDetectFilter.cpp

HEADERS += \
        src/gui/FaceVideoDlg.h \
        src/filter/QCvFaceDetectFilter.h

FORMS += \
        src/gui/FaceVideoDlg.ui
