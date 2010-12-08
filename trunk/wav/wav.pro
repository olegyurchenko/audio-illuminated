#-------------------------------------------------
#
# Project created by QtCreator 2010-12-07T10:18:18
#
#-------------------------------------------------

QT       += core gui
QT       += multimedia

TARGET = wav
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../wav_file.cpp \
    ../waveform.cpp \
    ../audio_controller.cpp

HEADERS  += mainwindow.h \
    ../wav_file.h \
    ../waveform.h \
    ../audio_controller.h

FORMS    += mainwindow.ui

INCLUDEPATH += .\
  ../
