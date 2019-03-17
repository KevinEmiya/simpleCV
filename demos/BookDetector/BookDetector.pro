#-------------------------------------------------
#
# Project created by QtCreator 2018-03-09T11:37:15
#
#-------------------------------------------------

include(../QCvUtils/BuildSchema.pri)
include(../QCvUtils/QCvUtils.pri)

INCLUDEPATH += $$PWD \
               $$PWD/src
DEPENDPATH += $$PWD

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BookDetector
TEMPLATE = app

SOURCES += \
        src/main.cpp \
        src/gui/MainDlg.cpp \
    src/component/PatternDetector.cpp \
    src/filter/QCvMatchResultFilter.cpp \
    src/component/PatternTracker.cpp

HEADERS += \
        src/gui/MainDlg.h \
        src/component/Pattern.h \
    src/component/PatternDetector.h \
    src/filter/QCvMatchResultFilter.h \
    src/component/PatternTracker.h

FORMS += \
        src/gui/MainDlg.ui
