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
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QIcon>
#include <editor_window.h>
/*----------------------------------------------------------------------------*/
struct QtMetaObject : private QObject
 {
 public:
     static const QMetaObject *get()
         { return &static_cast<QtMetaObject*>(0)->staticQtMetaObject; }
 };
/*----------------------------------------------------------------------------*/
/**Run js script*/
/*----------------------------------------------------------------------------*/
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
/*Script function to register extension*/
/*----------------------------------------------------------------------------*/
QScriptValue registerExtension(QScriptContext *context, QScriptEngine *engine)
{
  qDebug() << "registerExtension";
  if(context->argumentCount() < 2)
    return context->throwError(QScriptContext::TypeError, "Invalud argument count in registerExtension(extension, call)");
  QScriptValue extension = context->argument(0);
  QScriptValue text = extension.property("title");
  QScriptValue call = context->argument(1);
  if(!text.isString() || !call.isFunction())
    return context->throwError(QScriptContext::TypeError, "Extension havn't property 'title' or have not function to call (prop 'call')");

  QObject *obj = mainWindow;
  if(obj != NULL)
    obj = obj->findChild<QMenuBar *>("menuBar");
  if(obj == NULL)
    return context->throwError(QScriptContext::UnknownError, "Can't found menuBar");

  QMenu *parent = NULL;

  QStringList parentList = text.toString().split('/', QString::SkipEmptyParts);
  QString title;
  if(parentList.size())
  {
    title = parentList.last();
    parentList.pop_back();
  }

  if(parentList.isEmpty())
    parentList << "Extensions";

  for(int i = 0; i < parentList.size(); i++)
  {
    parent = NULL;
    QString parentText = parentList[i];
    QObjectList::const_iterator end = obj->children().end();
    for(QObjectList::const_iterator it = obj->children().begin(); it != end; it++)
    {
      QMenu *menu = qobject_cast<QMenu *>(*it);
      if(menu != NULL && menu->title() == parentText)
      {
        parent = menu;
        obj = menu;
        break;
      }
    }

    if(parent == NULL)
    {
      QMenuBar *bar = qobject_cast<QMenuBar *>(obj);
      if(bar != NULL)
        parent = bar->addMenu(parentText);
      else
      {
        QMenu *menu = qobject_cast<QMenu *>(obj);
        if(menu != NULL)
          parent = menu->addMenu(parentText);
      }
      obj = parent;
    }
  }

  //TODO: Make script icon
  QAction *action = parent->addAction(QIcon(":/resources/exec.png"), title);
  qScriptConnect(action, SIGNAL(triggered()), extension, call);
  return engine->newQObject(action);
}
/*----------------------------------------------------------------------------*/
QScriptValue processEvents(QScriptContext *context, QScriptEngine *)
{
  if(context->argumentCount())
    QCoreApplication::processEvents(QEventLoop::AllEvents, context->argument(0).toInt32());
  else
    QCoreApplication::processEvents();

  return QScriptValue();
}
/*----------------------------------------------------------------------------*/
/**Registration internal types*/
/*----------------------------------------------------------------------------*/
void registerTypes(QScriptEngine *engine)
{
  QScriptValue app = engine->newQObject(qApp);
  app.setProperty("processEvents", engine->newFunction(processEvents));
  engine->globalObject().setProperty("application", app);
  //We initialize the script engine to have the Qt namespace, so that e.g., Qt.Key_Left will be available to script code.
  QScriptValue Qt = engine->newQMetaObject(QtMetaObject::get());
  engine->globalObject().setProperty("Qt", Qt);
  registerGuiTypes(engine);
  QAudioFormatPrototype::registerPrototype(engine);
  WavFileWrapper::registerWrapper(engine);
  EffectPropertiesWrapper::registerWrapper(engine);

  app.setProperty("registerExtension", engine->newFunction(registerExtension));
  engine->globalObject().setProperty("registerExtension", engine->newFunction(registerExtension));

  engine->globalObject().setProperty("editWindow", engine->newQObject(mainWindow));
  engine->globalObject().setProperty("mainWindow", engine->newQObject(mainWindow));
  engine->globalObject().setProperty("processEvents", engine->newFunction(processEvents));
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
    engine->globalObject().setProperty("scriptDir", QScriptValue(scriptsDir.absolutePath()));
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
