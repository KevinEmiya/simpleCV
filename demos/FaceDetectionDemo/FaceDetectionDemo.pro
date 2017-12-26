#-------------------------------------------------
#
# Project created by QtCreator 2017-12-26T22:28:01
#
#-------------------------------------------------

include(../QCvUtils/BuildSchema.pri)
include(../QCvUtils/QCvUtils.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FaceDetectionDemo
TEMPLATE = app

SOURCES += \
        main.cpp \
        FaceVideoDlg.cpp

HEADERS += \
        FaceVideoDlg.h

FORMS += \
        FaceVideoDlg.ui
