include(../QCvUtils/QCvUtils.pri)

INCLUDEPATH += $$PWD \
               $$PWD/src
DEPENDPATH += $$PWD

PKG_CONFIG = PKG_CONFIG_PATH=$$PWD/../../sdk/opencv_release/lib/pkgconfig pkg-config
CONFIG  += link_pkgconfig
PKGCONFIG += opencv

SOURCES += \
    $$PWD/src/filter/QCvGaussFilter.cpp \
    $$PWD/src/filter/QCvHisEqFilter.cpp

HEADERS  += \
    $$PWD/src/filter/QCvGaussFilter.h \
    $$PWD/src/filter/QCvHisEqFilter.h
