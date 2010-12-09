/*----------------------------------------------------------------------------*/
/**
* @pkg effect_controller
*/
/**
* Module for handle effect plugins.
*
* Load, collect and handle plugins.<br>
* Copyright (C) 2010 Oleg Yurchenko, Kiev, Ukraine.<br>
* started 09.12.2010 19:08:11<br>
* @pkgdoc effect_controller
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef EFFECT_CONTROLLER_H_1291914492
#define EFFECT_CONTROLLER_H_1291914492
/*----------------------------------------------------------------------------*/
#include <QObject>
#include <effect_plugin_if.h>
#include <QHash>
class EffectController : public QObject
{
  Q_OBJECT
private:
  typedef struct
  {
    ControllerPluginInterface *iface;
    QWidget *propertyPanel;
    EffectDemoPanel *demoPanel;
  } Controller;
  typedef QHash<int, Controller> ControllerMap;

  typedef struct
  {
    EffectPluginInterface *iface;
    EffectPropertyPanel *propertyPanel;
  } Effect;
  typedef QHash<int, Effect> EffectMap;

  ControllerMap m_controllers;
  EffectMap m_effects;

public:
  EffectController(QObject *parent = NULL);
  virtual ~EffectController();

  void loadPlugins();

};

/*----------------------------------------------------------------------------*/
#endif /*EFFECT_CONTROLLER_H_1291914492*/

