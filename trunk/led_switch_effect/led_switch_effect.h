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
#ifndef LED_SWITCH_EFFECT_H_1292258060
#define LED_SWITCH_EFFECT_H_1292258060
/*----------------------------------------------------------------------------*/
#include <effect_plugin_if.h>
class SimpleSwitchLedEffect : public QObject, public EffectPluginInterface
{
  Q_OBJECT
  Q_INTERFACES(EffectPluginInterface)
protected:
  int m_effectId;
  QString m_effectName;
  QString m_effectCathegory;
  QString m_effectDescription;
  QPixmap m_effectIcon;
public:
  SimpleSwitchLedEffect(QObject *parent = NULL);
  virtual ~SimpleSwitchLedEffect();
  virtual int effectId() {return m_effectId;}
  virtual const QString& effectName(){return m_effectName;}
  virtual const QString& effectCathegory(){return m_effectCathegory;}
  virtual const QString& effectDescription(){return m_effectDescription;}
  virtual const QPixmap& effectIcon(){return m_effectIcon;}
  virtual EffectPropertyPanel* createPropertyPanel(QWidget *parent);
};
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
#endif /*LED_SWITCH_EFFECT_H_1292258060*/

