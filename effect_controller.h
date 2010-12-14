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
#include <QList>
class EffectController : public QObject
{
  Q_OBJECT
public:
  typedef QList<EffectProperties> PropList;
  typedef QList<EffectProperties*> PropPointList;

  typedef struct
  {
    ControllerPluginInterface *iface;
    QWidget *propertyPanel;
    EffectDemoPanel *demoPanel;
    //QPixmap pixmap;
  } Controller;
  typedef QHash<int, Controller> ControllerMap;

  typedef struct
  {
    EffectPluginInterface *iface;
    EffectPropertyPanel *propertyPanel;
    //QPixmap pixmap;
  } Effect;
  typedef QHash<int, Effect> EffectMap;

private:
  ControllerMap m_controllers;
  EffectMap m_effects;

  PropList m_properties;
  int m_unique;

  PropPointList m_lastSelect;
  qint64 m_windowStart;
  qint64 m_windowSize;

  qint64 m_lastPlayPosition;

  void loadPlugins();

public:
  EffectController(QObject *parent = NULL);
  virtual ~EffectController();

  int unique() {return ++m_unique;}
  EffectProperties *newEffect(int effectId, int channel);
  PropPointList& selectEffects(qint64 startUs, qint64 sizeUs);
  ControllerMap& controllers(){return m_controllers;}
  EffectMap& effects(){return m_effects;}
  const QPixmap& effectIcon(int effectId);

protected slots:
  void onPlayPosition(qint64 samples);
  void onStartPlay();
  void onStopPlay();

};

extern EffectController* effectController;
/*----------------------------------------------------------------------------*/
#endif /*EFFECT_CONTROLLER_H_1291914492*/

