#-------------------------------------------------
#
# Project created by QtCreator 2017-11-29T14:36:45
#
#-------------------------------------------------

include(../QCvUtils/BuildSchema.pri)
include(../QCvUtils/QCvUtils.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CameraViewer
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/gui/CamDlg.cpp \
    src/gui/CapDlg.cpp

HEADERS  += \
    src/gui/CamDlg.h \
    src/gui/CapDlg.h

FORMS    += \
    src/gui/CamDlg.ui \
    src/gui/CapDlg.ui
