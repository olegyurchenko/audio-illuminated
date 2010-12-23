/*----------------------------------------------------------------------------*/
/**
* @pkg gui_qscript
*/
/**
* Поддержка основных ыиджетов в QScript.
*
* Регистрация виджетов для использования в QScript.
* (C) T&T, Kiev, Ukraine 2010.<br>
* started 26.03.2010 12:32:04<br>
* @pkgdoc gui_qscript
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef GUI_QSCRIPT_H_1269599524
#define GUI_QSCRIPT_H_1269599524
/*----------------------------------------------------------------------------*/
#include <QScriptEngine>
/**Регистрация основных QT виджетов*/
void registerGuiTypes(QScriptEngine *engine);
/*----------------------------------------------------------------------------*/
/* Только для истории
#define ADD_QWIDGET_WRAPER(type) \
class type ## Wrapper\
{\
public:\
  static QScriptValue objConstructor(QScriptContext *context, QScriptEngine *engine)\
  {\
    QWidget *parent = qobject_cast<QWidget *>(context->argument(0).toQObject());\
    QWidget *object = new type(parent);\
    return engine->newQObject(object, QScriptEngine::ScriptOwnership);\
  }\
};\
do\
{\
  QScriptValue ctor = engine->newFunction(type##Wrapper::objConstructor);\
  QScriptValue metaObject = engine->newQMetaObject(&type::staticMetaObject, ctor);\
  engine->globalObject().setProperty(#type, metaObject);\
} while(0)
*/
/*----------------------------------------------------------------------------*/
#endif /*GUI_QSCRIPT_H_1269599524*/

