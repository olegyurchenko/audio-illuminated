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
#include "gui_qscript.h"
#include <QUiLoader>
#include <QWidget>
#include <QtDebug>
#include <QLayout>
#include <QFile>
/*----------------------------------------------------------------------------*/
static QScriptValue widgetConstructor(QScriptContext *context, QScriptEngine *engine)
{
  QScriptValue self = context->callee();
  const QString className = self.property("className").toString();
  //qDebug() << "Call " << className;
  QUiLoader *loader = qobject_cast<QUiLoader*>(engine->globalObject().property("UiLoader").toQObject());
  if(loader == NULL)
  {
    loader = new QUiLoader(engine);
    engine->globalObject().setProperty("UiLoader", engine->newQObject(loader));
  }
  QWidget *parent = qobject_cast<QWidget *>(context->argument(0).toQObject());
  QWidget *widget = loader->createWidget(className, parent);

  return engine->newQObject(widget, QScriptEngine::ScriptOwnership);
}
/*----------------------------------------------------------------------------*/
static QScriptValue layoutConstructor(QScriptContext *context, QScriptEngine *engine)
{
  QScriptValue self = context->callee();
  const QString className = self.property("className").toString();
  //qDebug() << "Call " << className;
  QUiLoader *loader = qobject_cast<QUiLoader*>(engine->globalObject().property("UiLoader").toQObject());
  if(loader == NULL)
  {
    loader = new QUiLoader(engine);
    engine->globalObject().setProperty("UiLoader", engine->newQObject(loader));
  }
  QObject *parent = context->argument(0).toQObject();
  QLayout *layout = loader->createLayout(className, parent);

  return engine->newQObject(layout, QScriptEngine::ScriptOwnership);
}
/*----------------------------------------------------------------------------*/
/*Load .ui file**/
static QScriptValue uiLoad(QScriptContext *context, QScriptEngine *engine)
{
  QUiLoader *loader = qobject_cast<QUiLoader*>(engine->globalObject().property("UiLoader").toQObject());
  if(loader == NULL)
  {
    loader = new QUiLoader(engine);
    engine->globalObject().setProperty("UiLoader", engine->newQObject(loader));
  }

  if(!context->argumentCount())
    return context->throwError(QScriptContext::TypeError, "Invalid argument count for uiLoad(fileName, parent=0)");

  QString fileName = context->argument(0).toString();
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly))
    return context->throwError(QScriptContext::TypeError, QString("Error open file '%1'").arg(fileName));


  QWidget *parent = NULL;
  if(context->argumentCount() > 1)
    parent = qobject_cast<QWidget *>(context->argument(1).toQObject());

  QWidget *widget = loader->load(&file, parent);

  return engine->newQObject(widget, QScriptEngine::ScriptOwnership);
}
/*----------------------------------------------------------------------------*/
/**Регистрация основных QT виджетов*/
void registerGuiTypes(QScriptEngine *engine)
{
  QUiLoader loader;
  int size = loader.availableWidgets().size();

  for(int i = 0; i < size; i++)
  {
    const QString className = loader.availableWidgets().at(i);
    //qDebug() << "Register class " << className;

     QScriptValue ct = engine->newFunction(widgetConstructor);
     ct.setProperty("className", className);
     engine->globalObject().setProperty(className, ct);
  }

  size = loader.availableLayouts().size();

  for(int i = 0; i < size; i++)
  {
    const QString className = loader.availableLayouts().at(i);
    //qDebug() << "Register class " << className;

     QScriptValue ct = engine->newFunction(layoutConstructor);
     ct.setProperty("className", className);
     engine->globalObject().setProperty(className, ct);
  }

  engine->globalObject().setProperty("uiLoad", engine->newFunction(uiLoad));
}
/*----------------------------------------------------------------------------*/

