#-------------------------------------------------
#
# Project created by QtCreator 2010-12-13T15:46:57
#
#-------------------------------------------------

CONFIG += plugin

TARGET = led_controller
TEMPLATE = lib

DEFINES += LED_CONROLLER_LIBRARY

SOURCES += led_controller.cpp \
    demo_panel.cpp

HEADERS += led_controller.h \
    ../effect_plugin_if.h \
    demo_panel.h

INCLUDEPATH += . \
    ..

RESOURCES += \
    resource.qrc
