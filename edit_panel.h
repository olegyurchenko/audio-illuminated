/*----------------------------------------------------------------------------*/
/**
* @pkg edit_panel
*/
/**
* Edit panel to host waveform + channels edit + scroll.
*
* Copyright (C) 2010 Oleg Yurchenko, Kiev, Ukraine.<br>
* started 10.12.2010 10:23:56<br>
* @pkgdoc edit_panel
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef EDIT_PANEL_H_1291969436
#define EDIT_PANEL_H_1291969436
/*----------------------------------------------------------------------------*/
#include <QWidget>
#include <waveform.h>
#include <channel_edit.h>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QVector>

class EditPanel : public QWidget
{
  Q_OBJECT
private:
  QVBoxLayout* vbLayout;
  WaveFormWidget *waveForm;
  QScrollBar *hScroll;
  typedef QVector<ChannelEdit *> EditVector;
  EditVector edits;

public:
  EditPanel(QWidget *parent);
  virtual ~EditPanel();
};

/*----------------------------------------------------------------------------*/
#endif /*EDIT_PANEL_H_1291969436*/

