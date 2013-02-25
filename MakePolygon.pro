#-------------------------------------------------
#
# Project created by QtCreator 2013-02-24T18:05:25
#
#-------------------------------------------------

QT       += core gui

TARGET = MakePolygon
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += "/usr/include/opencv/"

CONFIG += link_pkgconfig
PKGCONFIG += opencv

LIBS += -Icv -Ihighgui
