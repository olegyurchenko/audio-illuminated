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
#include "effect_controller.h"
#include <QPluginLoader>
#include <QDir>
#include <QApplication>
#include <audio_controller.h>

EffectController* effectController = NULL;
/*----------------------------------------------------------------------------*/
EffectController :: EffectController(QObject *parent)
  : QObject(parent)
{
  m_windowStart = -1;
  m_windowSize = -1;
  connect(audioController, SIGNAL(playPosition(qint64)), this, SLOT(onPlayPosition(qint64)));
  connect(audioController, SIGNAL(startPlay()), this, SLOT(onStartPlay()));
  connect(audioController, SIGNAL(stopPlay()), this, SLOT(onStopPlay()));
  //!!DEBUG
  newEffect(1, 1)->setTimeStart(20000);
  newEffect(1, 1)->setTimeStart(150000);
  newEffect(1, 1)->setTimeStart(300000);
  //!!DEBUG
  loadPlugins();
}
/*----------------------------------------------------------------------------*/
EffectController :: ~EffectController()
{
}
/*----------------------------------------------------------------------------*/
void EffectController :: loadPlugins()
{
  QDir pluginsDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
  if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
    pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
  if (pluginsDir.dirName() == "MacOS")
  {
    pluginsDir.cdUp();
    pluginsDir.cdUp();
    pluginsDir.cdUp();
  }
#endif
  pluginsDir.cd("plugins");
  foreach (QString fileName, pluginsDir.entryList(QDir::Files))
  {
    QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
    QObject *plugin = pluginLoader.instance();
    if(plugin)
    {
      EffectPluginInterface *effect = qobject_cast<EffectPluginInterface *>(plugin);
      if(effect != NULL)
      {
        Effect data;
        data.iface = effect;
        data.propertyPanel = NULL;
        m_effects[data.iface->effectId()] = data;
        continue;
      }
      ControllerPluginInterface *ctrl = qobject_cast<ControllerPluginInterface *>(plugin);
      if(ctrl != NULL)
      {
        Controller data;
        data.iface = ctrl;
        data.demoPanel = NULL;
        data.propertyPanel = NULL;
        m_controllers[data.iface->controllerId()] = data;
        continue;
      }
    }
  }
}
/*----------------------------------------------------------------------------*/
EffectProperties* EffectController :: newEffect(int effectId, int channel)
{
  EffectProperties prop(effectId, unique(), channel);
  m_properties.append(prop);
  return &m_properties.last();
}
/*----------------------------------------------------------------------------*/
EffectController::PropPointList& EffectController :: selectEffects(qint64 startUs, qint64 sizeUs)
{
  if(startUs == m_windowStart && sizeUs == m_windowSize)
    return m_lastSelect;
  m_windowStart = startUs;
  m_windowSize = sizeUs;
  m_lastSelect.clear();

  qint64 endUs = startUs + sizeUs;
  PropList::iterator end = m_properties.end();
  for(PropList::iterator it = m_properties.begin(); it != end; it++)
  {
    qint64 t = it->timeStart();
    if(t >= startUs && t < endUs)
      m_lastSelect.append(&*it);
  }
  return m_lastSelect;
}
/*----------------------------------------------------------------------------*/
void EffectController :: onPlayPosition(qint64 samples)
{
  qint64 position = audioController->quants2duration(samples);

  PropList::iterator end = m_properties.end();
  for(PropList::iterator it = m_properties.begin(); it != end; it++)
  {
    qint64 t = it->timeStart();
    if(t > m_lastPlayPosition && t <= position)
    {
      ControllerMap::iterator e = m_controllers.end();
      for(ControllerMap::iterator ci = m_controllers.begin(); ci != e; ci++)
      {
        if(ci.value().demoPanel != NULL)
          ci.value().demoPanel->effectStart(*it);
        ci.value().iface->effectStart(*it);
      }
    }
  }

  m_lastPlayPosition = position;
}
/*----------------------------------------------------------------------------*/
void EffectController :: onStartPlay()
{
  m_lastPlayPosition = 0;
}
/*----------------------------------------------------------------------------*/
void EffectController :: onStopPlay()
{
}
/*----------------------------------------------------------------------------*/
