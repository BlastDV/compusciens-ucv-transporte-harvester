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
    logreporter.cpp

HEADERS  += mainwindow.h \
    sessionwindow.h \
    logmaster.h \
    dbconnector.h \
    logreporter.h

FORMS    += mainwindow.ui \
    sessionwindow.ui \
    logreporter.ui

RESOURCES += \
    icons.qrc \
    pictures.qrc

OTHER_FILES += \
    Csp32.def

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/include/ -lCsp32

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include
