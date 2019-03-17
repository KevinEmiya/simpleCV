#-------------------------------------------------
#
# Project created by QtCreator 2018-03-07T15:51:37
#
#-------------------------------------------------

include(../QCvUtils/BuildSchema.pri)
include(../QCvUtils/QCvUtils.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CameraCalibration
TEMPLATE = app

SOURCES += \
        src/gui/main.cpp \
        src/gui/MainDlg.cpp \
    src/filter/QCvEdgePtFilter.cpp \
    src/component/CamCalibrator.cpp

HEADERS += \
        src/gui/MainDlg.h \
    src/filter/QCvEdgePtFilter.h \
    src/component/CamCalibrator.h

FORMS += \
        src/gui/MainDlg.ui
