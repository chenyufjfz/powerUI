#-------------------------------------------------
#
# Project created by QtCreator 2019-01-06T15:30:13
#
#-------------------------------------------------

QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport

Release:TARGET = PowerUI
Debug:TARGET = PowerUID
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    console.cpp \
    settingsdialog.cpp \
    datasource.cpp \
    datamodel.cpp \
    curveview.cpp \
    curveviewdlg.cpp

HEADERS  += mainwindow.h \
    console.h \
    settingsdialog.h \
    datasource.h \
    datamodel.h \
    curveview.h \
    curveviewdlg.h

FORMS    += mainwindow.ui \
    console.ui \
    settingsdialog.ui \
    curveviewdlg.ui

DESTDIR = $$_PRO_FILE_PWD_/app

LIBS += -lDbghelp -ladvapi32

RESOURCES += \
    images.qrc
