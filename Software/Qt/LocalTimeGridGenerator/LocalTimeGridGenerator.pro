#-------------------------------------------------
#
# Project created by QtCreator 2014-02-10T10:05:29
#
#-------------------------------------------------

QT += core gui
QT += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LocalTimeGridGenerator
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    grid.cpp \
    timezones.cpp \
    average.cpp

HEADERS  += mainwindow.h \
    grid.h \
    timezones.h \
    average.h \
    global.h

FORMS    += mainwindow.ui

DISTFILES +=

RESOURCES += \
    resources.qrc
