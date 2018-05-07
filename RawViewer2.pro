#-------------------------------------------------
#
# Project created by QtCreator 2018-04-11T23:39:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RawViewer2
TEMPLATE = app

LIBS += \
    -lswscale   \
    -lavformat  \
    -lavcodec   \
    -lavutil

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS __STDC_CONSTANT_MACROS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        src/main.cpp \
        src/mainwindow.cpp \
        src/frame.cpp \
        src/color_space.cpp \
        src/rawfile.cpp

HEADERS += \
        src/mainwindow.h \
        src/frame.h \
        src/rawfile.h

FORMS += \
        ui/mainwindow.ui   \
        ui/frame.ui

RESOURCES += \
        ui/toolbar.qrc
