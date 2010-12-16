/*----------------------------------------------------------------------------*/
/**
* @pkg project_file
*/
/**
* Functions to save & load illumination project.
*
* Copyright (C) 2010 Oleg Yurchenko, Kiev, Ukraine.<br>
* started 15.12.2010 17:34:44<br>
* @pkgdoc project_file
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "project_file.h"
#include <QtDebug>
#include <QDataStream>
#include <QFile>
#include <QVariantMap>
#include <QRegExp>
#include <QtDebug>
#include <QTemporaryFile>
#include <QApplication>
#include <QStringList>
#include <QDir>
/*----------------------------------------------------------------------------*/
QDataStream& operator << (QDataStream& out, const EffectProperties& p)
{
  out << p.effectId() << p.id() << p.channel() << p.timeStart() << p.properties();
  return out;
}
/*----------------------------------------------------------------------------*/
QDataStream& operator >> (QDataStream& in, EffectProperties& p)
{
  p.props().clear();
  int effectId, id, channel;
  qint64 timeStart;
  in >> effectId >> id >> channel >> timeStart >>p.props();
  p.setEffectId(effectId);
  p.setId(id);
  p.setChannel(channel);
  p.setTimeStart(timeStart);
  p.setModified(false);
  return in;
}
/*----------------------------------------------------------------------------*/

const QString projectHeader = QString("Illumination project v(1.0)");

bool saveProject(const QString& fileName, Project *project)
{
  QFile data(fileName);
  if (data.open(QFile::WriteOnly | QFile::Truncate))
  {
    QDataStream out(&data);
    out << projectHeader;
    out << project->wavFileName;
    int size = project->props->size();
    out << size;
    for(int i = 0; i < size; i++)
    {
      out << *(project->props->begin() + i);
    }

    QFile wav(project->wavFileName);
    if(wav.open(QFile::ReadOnly))
    {
      while(!wav.atEnd())
      {
        QByteArray array = wav.read(100 * 1024);
        out.writeRawData(array.constData(), array.size());
      }
    }
    return true;
  }
  return false;
}
/*----------------------------------------------------------------------------*/
bool loadProject(const QString& fileName, Project *project)
{
  QFile data(fileName);
  if (data.open(QFile::ReadOnly))
  {
    QDataStream in(&data);
    QString str;
    in >> str;
    if(str != projectHeader)
    {
      qDebug() << "Invalid stream file" << fileName;
      return false;
    }
    in >> project->wavFileName;
    project->props->clear();
    int size;
    in >> size;
    for(int i = 0; i < size; i++)
    {
      EffectProperties p(-1, -1, -1);
      in >> p;
      project->props->append(p);
    }

    if(!QFile::exists(project->wavFileName))
    {
      QString tempMask = QDir::tempPath();
      if(!tempMask.endsWith('/') && !tempMask.endsWith('\\'))
        tempMask += "/";
      tempMask += "illumination-edit-XXXXX";
      QTemporaryFile *file = new QTemporaryFile(tempMask, qApp);
      file->setAutoRemove(true);
      if(file->open())
      {
        char buffer[100 * 1024];
        while(!in.atEnd())
        {
          int r = in.readRawData(buffer, sizeof(buffer));
          if(r <= 0)
            break;
          file->write(buffer, r);
        }
        project->wavFileName = file->fileName();
        //qDebug() << "Using file" << project->wavFileName;
      }
    }
    return true;
  }
  return false;
}
/*----------------------------------------------------------------------------*/
