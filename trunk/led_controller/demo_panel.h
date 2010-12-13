/*----------------------------------------------------------------------------*/
/**
* @pkg demo_panel
*/
/**
* Demonstration panel for led controller.
*
* Copyright (C) 2010 Oleg Yurchenko, Kiev, Ukraine.<br>
* started 13.12.2010 16:39:40<br>
* @pkgdoc demo_panel
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef DEMO_PANEL_H_1292251181
#define DEMO_PANEL_H_1292251181
/*----------------------------------------------------------------------------*/
#include <effect_plugin_if.h>
#include <QWidget>
#include <QColor>

class DemoPanel : public EffectDemoPanel
{
protected:
  qint32 m_value;
  QColor m_bgColor;
  QColor m_fgColor;
  QColor m_ledColor;
public:
  DemoPanel(QWidget *parent);
  virtual ~DemoPanel();
  virtual void effectStart(const EffectProperties &prop);
protected:
  virtual void paintEvent(QPaintEvent *event);
};
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
#endif /*DEMO_PANEL_H_1292251181*/

