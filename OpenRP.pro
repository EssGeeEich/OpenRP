#-------------------------------------------------
#
# Project created by QtCreator 2017-01-28T20:31:21
#
#-------------------------------------------------

QT       += core gui

VERSION_MAJOR = 0
VERSION_MINOR = 1
VERSION_PATCH = 0
DEFINES += VERSION_MAJOR=$$VERSION_MAJOR VERSION_MINOR=$$VERSION_MINOR VERSION_PATCH=$$VERSION_PATCH

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OpenRP
TEMPLATE = app
LIBS += -lopenal -lvorbisfile -lassimp


SOURCES += main.cpp\
        startupwindow.cpp \
    gamewindow.cpp \
    gl/drawable.cpp \
    gl/material.cpp \
    gl/misc.cpp \
    gl/model.cpp \
    gl/object.cpp \
    gl/objectbone.cpp \
    gl/shader.cpp \
    gl/texture.cpp \
    al/context.cpp \
    al/device.cpp \
    al/devicelist.cpp \
    al/emitter.cpp \
    al/loader.cpp \
    link_enums.cpp \
    link.cpp

HEADERS  += startupwindow.h \
    gamewindow.h \
    gamemode.h \
    shared.h \
    link.h \
    gl/all.h \
    gl/drawable.h \
    gl/material.h \
    gl/misc.h \
    gl/model.h \
    gl/object.h \
    gl/objectbone.h \
    gl/shader.h \
    gl/shared.h \
    gl/texture.h \
    al/all.h \
    al/context.h \
    al/device.h \
    al/devicelist.h \
    al/emitter.h \
    al/enums.h \
    al/loader.h \
    al/shared.h

FORMS    += startupwindow.ui


include(../../Repository/LuaPP/qt_luapp.pri)

RESOURCES += \
    resources.qrc
