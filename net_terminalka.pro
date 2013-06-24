#-------------------------------------------------
#
# Project created by QtCreator 2013-05-11T19:05:44
#
#-------------------------------------------------

QT       += core gui serialport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = net_terminalka
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    myclient.cpp \
    myserver.cpp

HEADERS  += widget.h \
    myserver.h \
    myclient.h

FORMS    += widget.ui
