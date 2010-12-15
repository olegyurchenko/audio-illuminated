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
#include <QTextStream>
#include <QFile>
#include <QVariantMap>
#include <QRegExp>
#include <QtDebug>
/*----------------------------------------------------------------------------*/
const QString projectHeader = QString("Illumination project v(1.0)");
bool saveProject(const QString& fileName, Project *project)
{
  QFile data(fileName);
  if (data.open(QFile::WriteOnly | QFile::Truncate))
  {
    QTextStream out(&data);
    out << QString("[%1]\n").arg(projectHeader);
    out << QString("%1=%2\n").arg("wavFile", project->wavFileName);
    int size = project->props->size();
    for(int i = 0; i < size; i++)
    {
      EffectProperties* prop = &*(project->props->begin() + i);
      out << QString("[%1]\n").arg("effect");
      out << "id" << "=" << prop->id() << endl;
      out << "effectId" << "=" << prop->effectId() << endl;
      out << "channel" << "=" << prop->channel() << endl;
      out << "timeStart" << "=" << prop->timeStart() << endl;

      out << QString("[%1]\n").arg("effectProperty");
      QVariantMap::iterator end = prop->properties().end();
      for(QVariantMap::iterator it = prop->properties().begin(); it != end; it++)
        out << it.key() << "=" << it.value().toString() << endl;

    }
    out << QString("[end]\n");
    return true;
  }
  return false;
}
/*----------------------------------------------------------------------------*/
QString getSection(const QString& txt)
{
  return txt.mid(1,txt.indexOf("]") - 1);
}
/*----------------------------------------------------------------------------*/
QString getEquation(const QString& txt, QString *value)
{
  QString var;
  var = txt.left(txt.indexOf('=')).trimmed();
  *value = txt.mid(txt.indexOf('=')+1).trimmed();
  //qDebug() << "equation" << var << *value;
  return var;
}
/*----------------------------------------------------------------------------*/
bool loadProject(const QString& fileName, Project *project)
{
  enum
  {
    outside,
    inProject,
    inEffect,
    inProp
  } state = outside;

  QFile data(fileName);
  if (data.open(QFile::ReadOnly))
  {
    QTextStream in(&data);
    QRegExp sect("^\\[.+\\]");
    QRegExp equ("[A-Za-z]+=.*");
    QString line;
    QString txt;

    EffectProperties *prop = NULL;
    do
    {
      line = in.readLine();
      //qDebug() << "line" << line << line.indexOf(sect) << line.indexOf(equ);
      if(line.indexOf(sect) >= 0)
      {
        txt = getSection(line);
        //qDebug() << "section" << txt;
        if(state == outside && txt == projectHeader)
        {
          state = inProject;
          //qDebug() << "inProject";
        }
        else
        if((state == inProject || state == inProp) && txt == "effect")
        {
          state = inEffect;
          project->props->push_back(EffectProperties(-1, -1, -1));
          prop = &project->props->last();
        }
        else
        if(state == inEffect && txt == "effectProperty")
          state = inProp;
        else
        if(txt == "end")
          break;
      }
      else
      if(line.indexOf(equ) >= 0)
      {
        QString var = getEquation(line, &txt);
        if(state == inProject && var == "wavFile")
          project->wavFileName = txt;
        else
        if(state == inEffect && prop != NULL)
        {
          if(var == "id")
            prop->setId(txt.toInt());
          else
          if(var == "effectId")
            prop->setEffectId(txt.toInt());
          else
          if(var == "channel")
            prop->setChannel(txt.toInt());
          else
          if(var == "timeStart")
            prop->setTimeStart(txt.toInt());
        }
        else
        if(state == inProp && prop != NULL)
          prop->propertySet(var, txt);
      }

    } while (!line.isNull());
    return true;
  }
  return false;
}
/*----------------------------------------------------------------------------*/
