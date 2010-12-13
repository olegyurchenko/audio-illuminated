#-------------------------------------------------
#
# Project created by QtCreator 2010-12-13T18:32:48
#
#-------------------------------------------------

CONFIG += plugin

TARGET = led_switch_effect
TEMPLATE = lib

DEFINES += LED_SWITCH_EFFECT_LIBRARY

SOURCES += led_switch_effect.cpp \
    property_panel.cpp

HEADERS += led_switch_effect.h \
    ../effect_plugin_if.h \
    property_panel.h

INCLUDEPATH += . \
    ..

RESOURCES += \
    resource.qrc
