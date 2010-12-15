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
#ifndef PROJECT_FILE_H_1292427285
#define PROJECT_FILE_H_1292427285
/*----------------------------------------------------------------------------*/
#include <effect_controller.h>
typedef struct
{
  QString wavFileName;
  EffectController::PropList* props;
} Project;

bool saveProject(const QString& fileName, Project *project);
bool loadProject(const QString& fileName, Project *project);
/*----------------------------------------------------------------------------*/
#endif /*PROJECT_FILE_H_1292427285*/
