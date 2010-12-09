/*----------------------------------------------------------------------------*/
/**
* @pkg effect_plugin_if
*/
/**
* Interface classes for effect.
*
* Virtual base classes for illumination effects:<br>
* EffectProperties - Class represented effect propertyes<br>
* EffectPropertyPanel - Base class for effect property editor panel.<br>
* EffectDemoPanel - Base class for effect demonstartion panel.<br>
* Copyright (C) 2010 Oleg Yurchenko, Kiev, Ukraine.<br>
* started 09.12.2010 12:04:56<br>
* @pkgdoc effect_plugin_if
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef EFFECT_PLUGIN_IF_H_1291889096
#define EFFECT_PLUGIN_IF_H_1291889096
/*----------------------------------------------------------------------------*/
#include <QtPlugin>
#include <QString>
#include <QPixmap>
#include <QWidget>
#include <QVariant>
/*----------------------------------------------------------------------------*/
/**Class represented effect propertyes*/
class EffectProperties
{
protected:
  bool m_modified;
  int m_effectId; //Effect ID
  int m_id; //unique effect ID
  qint64 m_timeStart; //start time in us
  QVariantMap m_properties;

public:
  bool modified() const {return m_modified;}
  int effectId() const {return m_effectId;}
  int id() const {return m_id;}
  qint64 timeStart() const {return m_timeStart;}
  const QVariant property (const QString& name) const {return m_properties[name];}
  void propertySet(const QString& name, const QVariant& value) {m_properties[name] = value; m_modified = true;}
  void setModified(bool m) {m_modified = m;}

protected:
  void setEffectId(int i) {m_effectId = i;}
  void setId(int i) {m_id = i;}
  void setTimeStart(qint64 s) {m_timeStart = s;}
  QVariantMap& properties() {return m_properties;}
};
/*----------------------------------------------------------------------------*/
class EffectPropertyPanel
{
public:
  virtual ~EffectPropertyPanel(){}
  virtual void propertyEdit(EffectProperties *prop) = 0;
};
/*----------------------------------------------------------------------------*/
class EffectDemoPanel
{
public:
  virtual ~EffectDemoPanel(){}
  virtual void effectStart(const EffectProperties &prop) = 0;
};
/*----------------------------------------------------------------------------*/
class EffectPluginInterface
{
public:
  virtual ~EffectPluginInterface() {}
  virtual int effectId() = 0;
  virtual const QString& effectName() = 0;
  virtual const QString& effectCathegory() = 0;
  virtual const QString& effectDescription() = 0;
  virtual const QPixmap& effectIcon() = 0;
  virtual EffectPropertyPanel* createPropertyPanel(QWidget *parent) = 0;
};
/*----------------------------------------------------------------------------*/
class ControllerPluginInterface
{
public:
  virtual ~ControllerPluginInterface() {}
  virtual int controllerId() = 0;
  virtual const QString& controllerName() = 0;
  virtual const QString& controllerDescription() = 0;
  virtual const QPixmap& controllerIcon() = 0;
  virtual void effectStart(const EffectProperties &prop) = 0;
  virtual QWidget* createPropertyPanel(QWidget *parent) = 0;
  virtual EffectDemoPanel* createDemoPanelPanel(QWidget *parent) = 0;
};

/*----------------------------------------------------------------------------*/
Q_DECLARE_INTERFACE(EffectPluginInterface, "com.illuminated.EffectPluginInterface/1.0" )
Q_DECLARE_INTERFACE(ControllerPluginInterface, "com.illuminated.ControllerPluginInterface/1.0" )
/*----------------------------------------------------------------------------*/
#endif /*EFFECT_PLUGIN_IF_H_1291889096*/
