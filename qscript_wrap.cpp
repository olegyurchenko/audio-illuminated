/*----------------------------------------------------------------------------*/
/**
* @pkg qscript_wrap
*/
/**
* QScript wrappers for illumination-edit.
*
* Copyright (C) 2010 Oleg Yurchenko, Kiev, Ukraine.<br>
* started 22.12.2010 19:08:45<br>
* @pkgdoc qscript_wrap
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "qscript_wrap.h"
#include <QFile>
#include <QApplication>
#include <QScriptEngine>
#include <QDir>
#include <QString>
#include <QtDebug>
#include <editor_window.h>
#include <audio_controller.h>
#include <effect_controller.h>
#include <gui_qscript.h>
#include <wav_qscript.h>
#include <effect_qscript.h>
#include <QtScript>

/*----------------------------------------------------------------------------*/
struct QtMetaObject : private QObject
 {
 public:
     static const QMetaObject *get()
         { return &static_cast<QtMetaObject*>(0)->staticQtMetaObject; }
 };
/*----------------------------------------------------------------------------*/
/**Registration internal types*/
void registerTypes(QScriptEngine *engine);
/**Run js script*/
QScriptValue evaluateFile(QScriptEngine *engine, const QString &fileName);
/*----------------------------------------------------------------------------*/
#define ADD_QOBJECT_WRAPER(type) \
class type ## Wrapper\
{\
public:\
  static QScriptValue objConstructor(QScriptContext *context, QScriptEngine *engine)\
  {\
    QObject *parent = qobject_cast<QObject *>(context->argument(0).toQObject());\
    QObject *object = new type(parent);\
    return engine->newQObject(object, QScriptEngine::ScriptOwnership);\
  }\
};\
do\
{\
  QScriptValue ctor = engine->newFunction(type##Wrapper::objConstructor);\
  QScriptValue metaObject = engine->newQMetaObject(&type::staticMetaObject, ctor);\
  engine->globalObject().setProperty(#type, metaObject);\
} while(0)
/*----------------------------------------------------------------------------*/
/**Run js script*/
QScriptValue evaluateFile(QScriptEngine *engine, const QString &fileName)
{
  QFile file(fileName);
  if(!file.open(QIODevice::ReadOnly))
  {
    return engine->evaluate(QString("throw \"Error open file %1\"").arg(fileName));
  }
  return engine->evaluate(file.readAll(), fileName);
}
/*----------------------------------------------------------------------------*/
/**Registration internal types*/
void registerTypes(QScriptEngine *engine)
{
  QScriptValue app = engine->newQObject(qApp);
  engine->globalObject().setProperty("application", app);
  //We initialize the script engine to have the Qt namespace, so that e.g., Qt.Key_Left will be available to script code.
  QScriptValue Qt = engine->newQMetaObject(QtMetaObject::get());
  engine->globalObject().setProperty("Qt", Qt);
  registerGuiTypes(engine);
  QAudioFormatPrototype::registerPrototype(engine);
  WavFileWrapper::registerWrapper(engine);
  EffectPropertiesWrapper::registerWrapper(engine);
}
/*----------------------------------------------------------------------------*/
void loadScripts(QObject *parent)
{
  QScriptEngine *engine = new QScriptEngine(parent);
  registerTypes(engine);
  QDir scriptsDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
  if (scriptsDir.dirName().toLower() == "debug" || scriptsDir.dirName().toLower() == "release")
    scriptsDir.cdUp();
#elif defined(Q_OS_MAC)
  if (scriptsDir.dirName() == "MacOS")
  {
    scriptsDir.cdUp();
    scriptsDir.cdUp();
    scriptsDir.cdUp();
  }
#endif
  if(scriptsDir.cd("scripts"))
  {
    QStringList filters;
    filters << "*.js" << "*.ies" << "*.qs";
    scriptsDir.setNameFilters(filters);
    foreach (QString fileName, scriptsDir.entryList(QDir::Files))
    {
      QScriptValue result = evaluateFile(engine, scriptsDir.absoluteFilePath(fileName));
      if (result.isError())
        qDebug() << QString("Script '%1' error: %2").arg(fileName, result.toString());
      else
      if(engine->hasUncaughtException())
        qDebug() << QString("Script '%1' error: %2").arg(fileName, engine->uncaughtException().toString());
    }
  }
}
/*----------------------------------------------------------------------------*/
