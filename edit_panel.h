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
#include <QScrollArea>

class EditPanel : public QWidget
{
  Q_OBJECT
  typedef QWidget Inherited;
private:
  QVBoxLayout* vbLayout;
  WaveFormWidget *m_waveForm;
  QScrollBar *hScroll;
//  typedef QVector<ChannelEdit *> EditVector;
//  EditVector edits;
  ChannelEdit *m_channelEdit;
  int m_channelId;

public:
  EditPanel(QWidget *parent);
  virtual ~EditPanel();
  //void newChannel();
  WaveFormWidget *waveForm() {return m_waveForm;}
protected slots:
  void onWavOpen(WavFile *);
  void onWavClose();
  void onPlayPosition(qint64 samples);
  void onStartPlay();
  void onStopPlay();
  void onWindowStartChanged(qint64 pos);
  void onScrollChanged(int pos);
public slots:
  void setPosition(qint64 samples);
  void setWindowDuration(qint64 duration);
  void setChannel(int c);

};

/*----------------------------------------------------------------------------*/
#endif /*EDIT_PANEL_H_1291969436*/

