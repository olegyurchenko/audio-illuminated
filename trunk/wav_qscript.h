/*----------------------------------------------------------------------------*/
/**
* @pkg wav_qscript
*/
/**
* QScript wrappers for work with wav_file.
*
* Copyright (C) 2010 Oleg Yurchenko, Kiev, Ukraine.<br>
* started 23.12.2010  8:06:51<br>
* @pkgdoc wav_qscript
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef WAV_QSCRIPT_H_1293084411
#define WAV_QSCRIPT_H_1293084411
/*----------------------------------------------------------------------------*/
#include <wav_file.h>
#include <QMetaType>
#include <QScriptEngine>
#include <QScriptClass>
#include <QFile>

Q_DECLARE_METATYPE(QAudioFormat);
Q_DECLARE_METATYPE(QAudioFormat*);

Q_DECLARE_METATYPE(WavFile*);
Q_DECLARE_METATYPE(QFile*);

class QAudioFormatPrototype
{
  //---------------------------
  static QScriptValue toString(QScriptContext *context, QScriptEngine *engine)
  {
    QAudioFormat *f = qscriptvalue_cast<QAudioFormat *>(context->thisObject());
    if(f == NULL)
      return context->throwError(QScriptContext::TypeError, "QAudioFormatPrototype: this object is not a QAudioFormatPrototype");
    return QScriptValue(QString("QAudioFormat"));
  }
  //---------------------------
  static QScriptValue byteOrder(QScriptContext *context, QScriptEngine *engine)
  {
    QAudioFormat *f = qscriptvalue_cast<QAudioFormat *>(context->thisObject());
    if(f == NULL)
      return context->throwError(QScriptContext::TypeError, "QAudioFormatPrototype: this object is not a QAudioFormatPrototype");

    return QScriptValue((int)f->byteOrder());
  }
  //---------------------------
  static QScriptValue channelCount(QScriptContext *context, QScriptEngine *engine)
  {
    QAudioFormat *f = qscriptvalue_cast<QAudioFormat *>(context->thisObject());
    if(f == NULL)
      return context->throwError(QScriptContext::TypeError, "QAudioFormatPrototype: this object is not a QAudioFormatPrototype");

    return QScriptValue(f->channelCount());
  }
  //---------------------------
  static QScriptValue codec(QScriptContext *context, QScriptEngine *engine)
  {
    QAudioFormat *f = qscriptvalue_cast<QAudioFormat *>(context->thisObject());
    if(f == NULL)
      return context->throwError(QScriptContext::TypeError, "QAudioFormatPrototype: this object is not a QAudioFormatPrototype");

    return QScriptValue(f->codec());
  }
  //---------------------------
  static QScriptValue sampleSize(QScriptContext *context, QScriptEngine *engine)
  {
    QAudioFormat *f = qscriptvalue_cast<QAudioFormat *>(context->thisObject());
    if(f == NULL)
      return context->throwError(QScriptContext::TypeError, "QAudioFormatPrototype: this object is not a QAudioFormatPrototype");

    return QScriptValue(f->sampleSize());
  }
  //---------------------------
  static QScriptValue sampleType(QScriptContext *context, QScriptEngine *engine)
  {
    QAudioFormat *f = qscriptvalue_cast<QAudioFormat *>(context->thisObject());
    if(f == NULL)
      return context->throwError(QScriptContext::TypeError, "QAudioFormatPrototype: this object is not a QAudioFormatPrototype");

    return QScriptValue((int)f->sampleType());
  }
  //---------------------------
public:
  static void registerPrototype(QScriptEngine *engine)
  {

    QScriptValue prototype = engine->newObject();
    prototype.setProperty("toString", engine->newFunction(toString));
    prototype.setProperty("byteOrder", engine->newFunction(byteOrder), QScriptValue::PropertyGetter);
    prototype.setProperty("channelCount", engine->newFunction(channelCount), QScriptValue::PropertyGetter);
    prototype.setProperty("channels", engine->newFunction(channelCount), QScriptValue::PropertyGetter);
    prototype.setProperty("codec", engine->newFunction(codec), QScriptValue::PropertyGetter);
    prototype.setProperty("sampleSize", engine->newFunction(sampleSize), QScriptValue::PropertyGetter);
    prototype.setProperty("sampleType", engine->newFunction(sampleType), QScriptValue::PropertyGetter);

    engine->setDefaultPrototype(qMetaTypeId<QAudioFormat>(), prototype);
  }
};
/*----------------------------------------------------------------------------*/
typedef WavFile* PWavFile;
class WavFileWrapper
{
protected:
  static QScriptValue seek(QScriptContext *context, QScriptEngine *engine)
  {
    WavFile *w = qobject_cast<WavFile *>(context->thisObject().toQObject());
    QFile *file = qscriptvalue_cast<QFile *>(context->thisObject().property("__file__"));

    if(w == NULL || file == NULL)
      return context->throwError(QScriptContext::TypeError, "WavFileWrapper: this object is not a WavFileWrapper");

    if(context->argumentCount() != 1)
      return context->throwError(QScriptContext::TypeError, "Invalid argument count in seek(saples)");

    if(!file->isOpen())
    {
      file->setFileName(w->fileName());
      if(!file->open(QIODevice::ReadOnly))
        return QScriptValue(false);

    }

    return QScriptValue(w->seek(context->argument(0).toInt32(), *file));
  }
  //---------------------------
  static QScriptValue read(QScriptContext *context, QScriptEngine *engine)
  {
    WavFile *w = qobject_cast<WavFile *>(context->thisObject().toQObject());
    QFile *file = qscriptvalue_cast<QFile *>(context->thisObject().property("__file__"));

    if(w == NULL || file == NULL)
      return context->throwError(QScriptContext::TypeError, "WavFileWrapper: this object is not a WavFileWrapper");

    int count = 1;
    if(context->argumentCount() >= 1)
      count = context->argument(0).toInt32();


    if(!file->isOpen())
    {
      file->setFileName(w->fileName());
      if(!file->open(QIODevice::ReadOnly))
        return engine->newArray();
      if(!w->seek(0, *file))
        return engine->newArray();
    }

    QScriptValue array = engine->newArray(count);
    for(int i = 0; i < count; i++)
    {
      QVector<qreal> res = w->readNext(*file);
      int size = res.size();
      QScriptValue a = engine->newArray(size);
      for(int j = 0; j < size; j++)
        a.setProperty(j, res[j]);
      array.setProperty(i, a);
    }

    return array;
  }
  //---------------------------
  static QScriptValue toScriptValue(QScriptEngine *engine, const PWavFile &wav)
  {
    WavFile* w = qobject_cast<WavFile*>(wav);
    if(w == NULL)
      return QScriptValue();

    QScriptValue obj = engine->newQObject(w);
    QFile *file = new QFile(w);
    obj.setProperty("seek", engine->newFunction(seek));
    obj.setProperty("read", engine->newFunction(read));
    obj.setProperty("__file__", engine->newQObject(file));

    return obj;
  }
  //-------------------------
  static void fromScriptValue(const QScriptValue &obj, PWavFile &w)
  {
    //????
  }
  //-------------------------
public:
  static void registerWrapper(QScriptEngine *engine)
  {
    qScriptRegisterMetaType(engine, toScriptValue, fromScriptValue);
  }
  //-------------------------

};
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
#endif /*WAV_QSCRIPT_H_1293084411*/

