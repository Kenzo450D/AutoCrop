#-------------------------------------------------
#
# Project created by QtCreator 2013-03-05T03:14:03
#
#-------------------------------------------------

QT       += core gui

TARGET = AutoCrop
TEMPLATE = app


SOURCES += main.cpp mainwindow.cpp AutoCrop.cpp

HEADERS  += mainwindow.h AutoCrop.h

FORMS    += mainwindow.ui

INCLUDEPATH += "/usr/include/opencv/"

CONFIG += link_pkgconfig
PKGCONFIG += opencv

LIBS += -Icv -Ihighgui
