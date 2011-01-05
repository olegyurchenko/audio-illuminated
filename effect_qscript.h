/*----------------------------------------------------------------------------*/
/**
* @pkg effect_qscript
*/
/**
* QScript interface objects for EffectController.
*
* Copyright (C) 2010 Oleg Yurchenko, Kiev, Ukraine.<br>
* started 24.12.2010 10:44:58<br>
* @pkgdoc effect_qscript
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef EFFECT_QSCRIPT_H_1293180299
#define EFFECT_QSCRIPT_H_1293180299
/*----------------------------------------------------------------------------*/
#include <QMetaType>
#include <QScriptEngine>
#include <QScriptClass>
#include <effect_plugin_if.h>
#include <effect_controller.h>

Q_DECLARE_METATYPE(EffectProperties);
Q_DECLARE_METATYPE(EffectProperties*);
Q_DECLARE_METATYPE(QList<EffectProperties>)
Q_DECLARE_METATYPE(QList<EffectProperties*>)

class EffectPropertiesWrapper
{
protected:
  //---------------------------
  static QScriptValue toScriptValue(QScriptEngine *engine, const EffectProperties &p)
  {
    QScriptValue obj = engine->newVariant(QVariant::fromValue((EffectProperties*)&p));
#if 1
    QScriptValue prototype = engine->globalObject().property("EffectPropertiesPrototype");
    if(!prototype.isValid())
    {
      prototype = engine->newObject();
      prototype.setProperty("toString", engine->newFunction(toString));
      prototype.setProperty("id", engine->newFunction(id), QScriptValue::PropertyGetter);
      prototype.setProperty("effectId", engine->newFunction(effectId), QScriptValue::PropertyGetter|QScriptValue::PropertySetter);
      prototype.setProperty("channel", engine->newFunction(channel), QScriptValue::PropertyGetter|QScriptValue::PropertySetter);
      prototype.setProperty("timeStart", engine->newFunction(timeStart), QScriptValue::PropertyGetter|QScriptValue::PropertySetter);
      prototype.setProperty("property", engine->newFunction(property), QScriptValue::PropertyGetter/*|QScriptValue::PropertySetter*/);
      prototype.setProperty("prop", engine->newFunction(property), QScriptValue::PropertyGetter/*|QScriptValue::PropertySetter*/);
      engine->globalObject().setProperty("EffectPropertiesPrototype", prototype);
    }
    obj.setPrototype(prototype);
#else
    obj.setProperty("toString", engine->newFunction(toString));
    obj.setProperty("id", engine->newFunction(id), QScriptValue::PropertyGetter);
    obj.setProperty("effectId", engine->newFunction(effectId), QScriptValue::PropertyGetter|QScriptValue::PropertySetter);
    obj.setProperty("channel", engine->newFunction(channel), QScriptValue::PropertyGetter|QScriptValue::PropertySetter);
    obj.setProperty("timeStart", engine->newFunction(timeStart), QScriptValue::PropertyGetter|QScriptValue::PropertySetter);
    obj.setProperty("property", engine->newFunction(property), QScriptValue::PropertyGetter/*|QScriptValue::PropertySetter*/);
#endif
    return obj;
  }
  //-------------------------
  static void fromScriptValue(const QScriptValue &obj, EffectProperties &p)
  {
    //????
  }
  //-------------------------
  typedef EffectProperties* PEffectProperties;
  //-------------------------
  static QScriptValue pointToScriptValue(QScriptEngine *engine, const PEffectProperties &p)
  {
    return toScriptValue(engine, *p);
  }
  //-------------------------
  static void pointFromScriptValue(const QScriptValue &obj, PEffectProperties &p)
  {
  }
  //-------------------------
  static QScriptValue ctor(QScriptContext *context, QScriptEngine *engine)
  {
    EffectProperties *p = effectController->newEffect(-1, -1);
    int count = context->argumentCount();
    if(count == 1)
    {
      EffectProperties *src = qscriptvalue_cast<EffectProperties *>(context->argument(0).toObject());
      if(src != NULL)
      {
        p->setChannel(src->channel());
        p->setEffectId(src->effectId());
        p->setTimeStart(src->timeStart());
        p->props() = src->properties();
        return toScriptValue(engine, *p);
      }
    }

    if(count >= 1)
      p->setEffectId(context->argument(0).toInt32());
    if(count >= 2)
      p->setChannel(context->argument(1).toInt32());

    return toScriptValue(engine, *p);
  }
  //---------------------------
  static QScriptValue toString(QScriptContext *context, QScriptEngine *engine)
  {
    EffectProperties *v = context->thisObject().toVariant().value<EffectProperties *>();
    //EffectProperties *v = qscriptvalue_cast<EffectProperties *>(context->thisObject());
    if(v == NULL)
      return context->throwError(QScriptContext::TypeError, "EffectProperties.toString: this object is not a EffectPropertiesWrapper");
    return QScriptValue(QString("Effect(id=%1,effectId=%2,channel=%3, timeStart=%4)").arg(
        QString::number(v->id()),
        QString::number(v->effectId()),
        QString::number(v->channel()),
        QString::number(v->timeStart())));
  }
  //---------------------------
  static QScriptValue id(QScriptContext *context, QScriptEngine *engine)
  {
    EffectProperties *v = context->thisObject().toVariant().value<EffectProperties *>();
    if(v == NULL)
      return context->throwError(QScriptContext::TypeError, "EffectPropertiesPrototypeg: this object is not a EffectPropertiesWrapper");

    return QScriptValue(v->id());
  }
  //---------------------------
  static QScriptValue effectId(QScriptContext *context, QScriptEngine *engine)
  {
    EffectProperties *v = context->thisObject().toVariant().value<EffectProperties *>();
    if(v == NULL)
      return context->throwError(QScriptContext::TypeError, "EffectPropertiesPrototypeg: this object is not a EffectPropertiesWrapper");

    if(context->argumentCount())
      v->setEffectId(context->argument(0).toInt32());

    return QScriptValue(v->effectId());
  }
  //---------------------------
  static QScriptValue channel(QScriptContext *context, QScriptEngine *engine)
  {
    EffectProperties *v = context->thisObject().toVariant().value<EffectProperties *>();
    if(v == NULL)
      return context->throwError(QScriptContext::TypeError, "EffectPropertiesPrototypeg: this object is not a EffectPropertiesWrapper");

    if(context->argumentCount())
      v->setChannel(context->argument(0).toInt32());

    return QScriptValue(v->channel());
  }
  //---------------------------
  static QScriptValue timeStart(QScriptContext *context, QScriptEngine *engine)
  {
    EffectProperties *v = context->thisObject().toVariant().value<EffectProperties *>();
    if(v == NULL)
      return context->throwError(QScriptContext::TypeError, "EffectPropertiesPrototypeg: this object is not a EffectPropertiesWrapper");

    if(context->argumentCount())
      v->setTimeStart(context->argument(0).toInt32());

    return QScriptValue((qint32)v->timeStart());
  }
  //---------------------------
  static QScriptValue property(QScriptContext *context, QScriptEngine *engine)
  {
    EffectProperties *v = context->thisObject().toVariant().value<EffectProperties *>();
    if(v == NULL)
      return context->throwError(QScriptContext::TypeError, "EffectPropertiesPrototypeg: this object is not a EffectPropertiesWrapper");

    if(!context->argumentCount())
      return context->throwError(QScriptContext::TypeError, "Invalud argument count in EffectProperties.property(name[, value])");

    if(context->argumentCount() >= 2)
      v->propertySet(context->argument(0).toString(), context->argument(1).toVariant());

    return engine->newVariant(v->property(context->argument(0).toString()));
  }
  //---------------------------
public:
  static void registerWrapper(QScriptEngine *engine)
  {

    qScriptRegisterMetaType(engine, toScriptValue, fromScriptValue);

    engine->globalObject().setProperty("EffectProperties", engine->newFunction(ctor));
    engine->globalObject().setProperty("Effect", engine->newFunction(ctor));

    qScriptRegisterMetaType(engine, pointToScriptValue, pointFromScriptValue);

    qScriptRegisterSequenceMetaType<QList<EffectProperties> >(engine);
    qScriptRegisterSequenceMetaType<QList<EffectProperties*> >(engine);
  }

};

/*----------------------------------------------------------------------------*/
#endif /*EFFECT_QSCRIPT_H_1293180299*/
