#-------------------------------------------------
#
# Project created by QtCreator 2014-05-11T15:05:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bludiste2014
TEMPLATE = app

DESTDIR = ./
OBJECTS_DIR = ./src/obj
MOC_DIR = ./src/obj
RCC_DIR = ./src/obj
UI_DIR = ./src/obj

LIBS += -static-libstdc++ -lboost_system  -lboost_thread-mt -lncurses

SOURCES += gui.cpp\
        mainwindow.cpp\
        client.cpp 

HEADERS  += mainwindow.h\
            client.h \
            gui.h

FORMS    += mainwindow.ui

RESOURCES += images.qrc
