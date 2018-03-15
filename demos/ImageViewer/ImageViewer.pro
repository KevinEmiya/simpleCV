#-------------------------------------------------
#
# Project created by QtCreator 2017-12-16T22:44:11
#
#-------------------------------------------------

include(../QCvUtils/BuildSchema.pri)
include(../QCvCommonFilters/QCvCommonFilters.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += $$PWD/src

TARGET = ImageViewer
TEMPLATE = app

SOURCES += \
        src/main.cpp \
        src/gui/ImageDlg.cpp

HEADERS += \
        src/gui/ImageDlg.h

FORMS += \
        src/gui/ImageDlg.ui
