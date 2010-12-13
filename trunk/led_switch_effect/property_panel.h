/*----------------------------------------------------------------------------*/
/**
* @pkg property_panel
*/
/**
* Property panel for led switch effect.
*
* Copyright (C) 2010 Oleg Yurchenko, Kiev, Ukraine.<br>
* started 13.12.2010 19:05:13<br>
* @pkgdoc property_panel
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef PROPERTY_PANEL_H_1292259913
#define PROPERTY_PANEL_H_1292259913
/*----------------------------------------------------------------------------*/
#include <effect_plugin_if.h>

class QPushButton;
class PropertyPanel : public EffectPropertyPanel
{
Q_OBJECT
protected:
  EffectProperties *m_prop;
  QPushButton *m_buttons[32];
public:
  PropertyPanel(QWidget *parent);
  virtual ~PropertyPanel();
  virtual void propertyEdit(EffectProperties *prop);
protected slots:
  void onBitToggled (bool checked);
};

/*----------------------------------------------------------------------------*/
#endif /*PROPERTY_PANEL_H_1292259913*/

