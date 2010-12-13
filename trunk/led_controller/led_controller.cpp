/*----------------------------------------------------------------------------*/
/**
* @pkg led_controller
*/
/**
* Simple 32 bit led controller.
*
* Copyright (C) 2010 Oleg Yurchenko, Kiev, Ukraine.<br>
* started 13.12.2010 15:48:11<br>
* @pkgdoc led_controller
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "led_controller.h"
#include "demo_panel.h"
#include <QtDebug>
/*----------------------------------------------------------------------------*/
Q_EXPORT_PLUGIN2(ControllerPluginInterface, LedController)

LedController :: LedController(QObject *parent)
  : QObject(parent)
{
  m_controllerName = tr("Led controller");
  m_controllerDescription = tr("Controller to controll 1 color guarland up to 32 led");
  m_controllerIcon = QPixmap(":/resources/led_ctrl.png");
  qDebug() << QString("%1 created").arg(m_controllerName);
}
/*----------------------------------------------------------------------------*/
LedController :: ~LedController()
{
}
/*----------------------------------------------------------------------------*/
void LedController :: effectStart(const EffectProperties &prop)
{
}
/*----------------------------------------------------------------------------*/
QWidget* LedController :: createPropertyPanel(QWidget *parent)
{
  return NULL;
}
/*----------------------------------------------------------------------------*/
EffectDemoPanel* LedController :: createDemoPanel(QWidget *parent)
{
  return new DemoPanel(parent);
}
/*----------------------------------------------------------------------------*/

