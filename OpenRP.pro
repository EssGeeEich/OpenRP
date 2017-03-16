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
LIBS += -lopenal -lvorbisfile


SOURCES += main.cpp\
        startupwindow.cpp \
    gamewindow.cpp \
    link_enums.cpp \
    link.cpp \
    rp_openal.cpp \
    rp_openal_ogg.cpp \
    rp_opengl.cpp

HEADERS  += startupwindow.h \
    gamewindow.h \
    gamemode.h \
    shared.h \
    link.h \
    rp_openal.h \
    rp_opengl.h

FORMS    += startupwindow.ui


include(../../Repository/LuaPP/qt_luapp.pri)

RESOURCES += \
    resources.qrc
