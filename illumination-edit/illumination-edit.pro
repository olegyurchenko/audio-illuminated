#-------------------------------------------------
#
# Project created by QtCreator 2010-12-09T12:08:38
#
#-------------------------------------------------

QT       += core gui
QT       += multimedia

TARGET = illumination-edit
TEMPLATE = app


SOURCES += main.cpp\
        editor_window.cpp \
    ../waveform.cpp \
    ../wav_file.cpp \
    ../audio_controller.cpp \
    ../effect_controller.cpp

HEADERS  += editor_window.h \
    ../waveform.h \
    ../wav_file.h \
    ../effect_plugin_if.h \
    ../audio_controller.h \
    ../effect_controller.h

FORMS    += editorwindow.ui

INCLUDEPATH += .\
  ../
