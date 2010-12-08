# -------------------------------------------------
# Project created by QtCreator 2010-11-26T10:16:38
# -------------------------------------------------
TARGET = record1d
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    led.cpp \
    led_effects.cpp \
    led_project.cpp 
HEADERS += mainwindow.h \
    led.h \
    led_effects.h \
    led_project.h 
FORMS += mainwindow.ui
win32:DEFINES += WIN32
!win32:DEFINES += LINUX
