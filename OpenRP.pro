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


SOURCES += main.cpp\
        startupwindow.cpp \
    gamewindow.cpp \
    luagl_api.cpp \
    lua_api.cpp \
    lua_ogl_link.cpp \
    lua_ogl_functions.cpp

HEADERS  += startupwindow.h \
    gamewindow.h \
    gamemode.h \
    lua_ogl_link.h

FORMS    += startupwindow.ui


include(../../Repository/LuaPP/qt_luapp.pri)

RESOURCES += \
    resources.qrc
