#-------------------------------------------------
#
# Project created by QtCreator 2018-11-29T15:45:37
#
#-------------------------------------------------

QT       += gui
QT       += widgets
TEMPLATE  = lib
CONFIG   += plugin
VERSION   = 0.0.1
TARGET    = $$qtLibraryTarget(cam)


GENERATED_DIR = ../../generated/plugin/cam
# Use common project definitions.
include(../../common.pri)

# For plugins
INCLUDEPATH    += ../../librecad/src/plugins \
                  ../../librecad/src/lib/engine \
                  ../../librecad/src/main

SOURCES += \
    camconfigure.cpp \
    camdlg.cpp \
    clipper.cpp \
    camtool.cpp


HEADERS  += \
    camconfigure.h \
    camdlg.h \
    clipper.hpp \
    cam_info.h \
    camtool.h


# Installation Directory
win32 {
        DESTDIR = ../../windows/resources/plugins
}
unix {
    macx {
        DESTDIR = ../../LibreCAD.app/Contents/Resources/plugins
    }
    else {
        DESTDIR = ../../unix/resources/plugins
    }
}
