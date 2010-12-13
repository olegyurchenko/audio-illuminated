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
#ifndef LED_CONTROLLER_H_1292248092
#define LED_CONTROLLER_H_1292248092
/*----------------------------------------------------------------------------*/
#include <effect_plugin_if.h>

class LedController : public QObject, public ControllerPluginInterface
{
  Q_OBJECT
  Q_INTERFACES(ControllerPluginInterface)
protected:
  QString m_controllerName;
  QString m_controllerDescription;
  QPixmap m_controllerIcon;
public:
  LedController(QObject *parent = NULL);
  virtual ~LedController();
  virtual int controllerId(){return 1;}
  virtual const QString& controllerName(){return m_controllerName;}
  virtual const QString& controllerDescription(){return m_controllerDescription;}
  virtual const QPixmap& controllerIcon() {return m_controllerIcon;}
  virtual void effectStart(const EffectProperties &prop);
  virtual QWidget* createPropertyPanel(QWidget *parent);
  virtual EffectDemoPanel* createDemoPanel(QWidget *parent);
};

/*----------------------------------------------------------------------------*/
#endif /*LED_CONTROLLER_H_1292248092*/

