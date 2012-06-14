#-------------------------------------------------
#
# Project created by QtCreator 2012-06-14T18:05:39
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = heightmap
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

OTHER_FILES += \
    noise.vert \
    noise.frag

RESOURCES += \
    data.qrc
