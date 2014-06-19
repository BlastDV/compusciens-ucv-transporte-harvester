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
    sessionwindow.cpp \
    logmaster.cpp \
    dbconnector.cpp \
    logreporter.cpp \
    appcontroller.cpp \
    csp32bridge.cpp

HEADERS  += mainwindow.h \
    sessionwindow.h \
    logmaster.h \
    dbconnector.h \
    logreporter.h \
    appcontroller.h \
    csp32bridge.h

FORMS    += mainwindow.ui \
    sessionwindow.ui \
    logreporter.ui

RESOURCES += \
    icons.qrc \
    pictures.qrc

win32: LIBS += -L$$PWD/include/ -lWSCLib

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include
