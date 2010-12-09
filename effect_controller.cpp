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
/*----------------------------------------------------------------------------*/
EffectController :: EffectController(QObject *parent)
  : QObject(parent)
{
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

