/*----------------------------------------------------------------------------*/
/**
* @pkg led_project
*/
/**
* Short description of led_project.
*
* Long description of led_project.<br>
* (C) T&T, Kiev, Ukraine 2010.<br>
* started 30.11.2010  9:19:39<br>
* @pkgdoc led_project
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "led_project.h"
#include <QSound>
#include <QFile>
#include <QtDebug>
#include <stdio.h>
#include <stdlib.h>
/*----------------------------------------------------------------------------*/
QDataStream& operator <<(QDataStream& out, const LedRecord& r)
{
  int size = r.leds.size();
  out << r.time;
  out << size;
  for(int i = 0; i < size; i++)
    out << r.leds[i];
  return out;
}
/*----------------------------------------------------------------------------*/
QDataStream& operator >>(QDataStream& in, LedRecord& r)
{
  int size = 0;
  in >> r.time;
  in >> size;
  for(int i = 0; i < size; i++)
  {
    int v;
    in >> v;
    r.leds.append(v);
  }
  return in;
}
/*----------------------------------------------------------------------------*/
LedProject :: LedProject(QObject *parent)
: QObject(parent)
{
  pmodified = false;
  sound = NULL;
}
/*----------------------------------------------------------------------------*/
LedProject :: ~LedProject()
{
}
/*----------------------------------------------------------------------------*/
void LedProject :: start()
{
  if(!pwavFile.isNull())
  {
    if(sound != NULL)
      delete sound;
#ifdef WIN32
    sound = new QSound(pwavFile, this);
#else
    char cmd[256];
    sprintf(cmd, "play %s > /dev/null 2>&1 &", pwavFile.toLocal8Bit().constData());
    system(cmd);
#endif
  }

  if(sound != NULL)
  {
    sound->play();
  }

  time.start();
}
/*----------------------------------------------------------------------------*/
void LedProject :: stop()
{
#ifdef WIN32
  if(sound != NULL)
  {
    sound->stop();
    delete sound;
    sound = NULL;
  }
#else
    char cmd[256];
    sprintf(cmd, "killall play  > /dev/null 2>&1");
    system(cmd);
    system("killall sox");
#endif
  emit stoped();
}
/*----------------------------------------------------------------------------*/
void LedProject :: addRecord(LedVector *leds) //Record
{
  LedRecord r;
  r.time = time.elapsed();
  LedVector::iterator end = leds->end();
  for(LedVector::iterator it = leds->begin(); it != end; it++)
    r.leds.append(it->value());
  recordList.append(r);
}
/*----------------------------------------------------------------------------*/
void LedProject :: getRecord(LedVector *leds) //Play
{
  LedRecord *r = NULL;
  int t = time.elapsed();
  if(recordList.isEmpty())
    return;
  LedRecordList::iterator end = recordList.end();
  for(LedRecordList::iterator it = recordList.begin(); it != end; it++)
  {
    r = &*it;
    if(r->time > t)
      break;
  }

  if(r == &recordList.back())
    stop();

  if(r != NULL)
  {
    int size1 = r->leds.size();
    int size2 = leds->size();
    for(int i = 0; i < size1 && i < size2; i++)
      (*leds)[i].setValue(r->leds[i]);
  }
}
/*----------------------------------------------------------------------------*/
bool LedProject :: readFromFile(const QString& fileName)
{
  pfileName = fileName;
  QFile file(pfileName);
  if(!file.open(QIODevice::ReadOnly))
    return false;
  QDataStream in(&file);
  in >> pwavFile;
  if(in.atEnd())
    return false;
  recordList.clear();
  in >> recordList;
  pmodified = false;
  return true;
}
/*----------------------------------------------------------------------------*/
bool LedProject :: writeToFile(const QString& fileName)
{
  if(!fileName.isNull())
    pfileName = fileName;
  QFile file(pfileName);
  if(!file.open(QIODevice::WriteOnly))
    return false;
  QDataStream out(&file);
  out << pwavFile;
  out << recordList;
  pmodified = false;
  return true;
}
/*----------------------------------------------------------------------------*/
bool LedProject :: isSoundAvailable()
{
#ifdef WIN32
  return QSound::isAvailable();
#else
  return true;
#endif
}
/*----------------------------------------------------------------------------*/
