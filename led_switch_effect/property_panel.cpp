/*----------------------------------------------------------------------------*/
/**
* @pkg property_panel
*/
/**
* Short description of property_panel.
*
* Long description of property_panel.<br>
* Copyright (C) 2010 Oleg Yurchenko, Kiev, Ukraine.<br>
* started 13.12.2010 19:05:13<br>
* @pkgdoc property_panel
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "property_panel.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
/*----------------------------------------------------------------------------*/
PropertyPanel :: PropertyPanel(QWidget *parent)
  : EffectPropertyPanel(parent)
{
  m_prop = NULL;
  QVBoxLayout *vbLayout = new QVBoxLayout(this);
  QHBoxLayout *hbLayout = new QHBoxLayout();

  vbLayout->addLayout(hbLayout);
  for(int i = 0; i < 32; i++)
  {
    if(i && !(i % 8))
    {
      hbLayout = new QHBoxLayout();
      vbLayout->addLayout(hbLayout);
    }

    QPushButton *btn = new QPushButton(QString::number(i), this);
    m_buttons[i] = btn;
    btn->setCheckable(true);
    btn->setFixedSize(24, 24);
    btn->setProperty("bit", i);

    connect(btn, SIGNAL(toggled(bool)), this, SLOT(onBitToggled(bool)));
    hbLayout->addWidget(btn);
  }

}
/*----------------------------------------------------------------------------*/
PropertyPanel :: ~PropertyPanel()
{
}
/*----------------------------------------------------------------------------*/
void PropertyPanel :: propertyEdit(EffectProperties *prop)
{
  m_prop = NULL;
  if(prop != NULL)
  {
    qint32 value = prop->property("value").toInt();
    //For first use
    prop->propertySet("value", value);
    prop->propertySet("led", 1); //For led controller
    for(int i = 0; i < 32; i++)
      m_buttons[i]->setChecked(value & (1 << i));
    m_prop = prop;
  }
}
/*----------------------------------------------------------------------------*/
void PropertyPanel :: onBitToggled(bool checked)
{
  if(m_prop == NULL)
    return;
  int bit = sender()->property("bit").toInt();
  qint32 value = m_prop->property("value").toInt();
  if(checked)
    value |= 1 << bit;
  else
    value &= ~(1 << bit);
  m_prop->propertySet("value", value);

}
/*----------------------------------------------------------------------------*/

