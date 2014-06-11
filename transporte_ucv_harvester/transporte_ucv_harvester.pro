#-------------------------------------------------
#
# Project created by QtCreator 2014-06-05T14:57:39
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = transporte_ucv_harvester
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    sessionwindow.cpp

HEADERS  += mainwindow.h \
    sessionwindow.h

FORMS    += mainwindow.ui \
    sessionwindow.ui

RESOURCES += \
    icons.qrc \
    pictures.qrc
