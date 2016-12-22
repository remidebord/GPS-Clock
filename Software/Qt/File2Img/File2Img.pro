#-------------------------------------------------
#
# Project created by QtCreator 2016-05-03T22:50:50
#
#-------------------------------------------------

QT += core gui
QT += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = File2Img
TEMPLATE = app

SOURCES += main.cpp
SOURCES += mainwindow.cpp
SOURCES += image.cpp
SOURCES += utils.cpp

HEADERS += mainwindow.h
HEADERS += image.h
HEADERS += utils.h

FORMS += mainwindow.ui

wince*: {
   DEPLOYMENT_PLUGIN += qjpeg qgif
}
