/*----------------------------------------------------------------------------*/
/**
* @pkg led_switch_effect
*/
/**
* Simple switch led effect.
*
* Copyright (C) 2010 Oleg Yurchenko, Kiev, Ukraine.<br>
* started 13.12.2010 18:34:19<br>
* @pkgdoc led_switch_effect
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include <QtDebug>
#include "led_switch_effect.h"
#include "property_panel.h"
/*----------------------------------------------------------------------------*/
Q_EXPORT_PLUGIN2(EffectPluginInterface, SimpleSwitchLedEffect)

SimpleSwitchLedEffect :: SimpleSwitchLedEffect(QObject *parent)
  :QObject(parent)
{
  m_effectId = 1;
  m_effectName = tr("Simple switch led");
  m_effectCathegory = tr("Led effects");
  m_effectDescription = tr("Simple switch led (on/off) on led controller");
  m_effectIcon = QPixmap(":/resources/led_switch.png");
  qDebug() << QString("%1 created").arg(m_effectName);
}
/*----------------------------------------------------------------------------*/
SimpleSwitchLedEffect :: ~SimpleSwitchLedEffect()
{
}
/*----------------------------------------------------------------------------*/
EffectPropertyPanel* SimpleSwitchLedEffect :: createPropertyPanel(QWidget *parent)
{
  return new PropertyPanel(parent);
}
/*----------------------------------------------------------------------------*/

