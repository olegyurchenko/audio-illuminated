/*----------------------------------------------------------------------------*/
/**
* @pkg edit_panel
*/
/**
* Edit panel to host waveform + channels edit + scroll.
*
* Long description of edit_panel.<br>
* Copyright (C) 2010 Oleg Yurchenko, Kiev, Ukraine.<br>
* started 10.12.2010 10:23:56<br>
* @pkgdoc edit_panel
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "edit_panel.h"
#include <audio_controller.h>
#include <QPalette>
/*----------------------------------------------------------------------------*/
EditPanel :: EditPanel(QWidget *parent)
  :QWidget(parent)
{
  vbLayout = new QVBoxLayout(this);
  vbLayout->setMargin(0);


  m_waveForm = new WaveFormWidget(this);
  m_waveForm->setBgColor(palette().color(QPalette::Window));
  m_waveForm->setFgColor(palette().color(QPalette::Foreground));
  m_waveForm->setMarkerColor(Qt::darkBlue);
  m_waveForm->setGridColor(Qt::darkGray);



  connect(audioController, SIGNAL(wavOpen(WavFile*)), m_waveForm, SLOT(wavFileOpened(WavFile*)));
  connect(audioController, SIGNAL(wavClose()), m_waveForm, SLOT(wavFileClosed()));
  connect(audioController, SIGNAL(playPosition(qint64)), m_waveForm, SLOT(setFilePosition(qint64)));

  for(int i = 0; i < 2; i++)
  {
    newChannel();
  }

  vbLayout->addWidget(m_waveForm);

  hScroll = new QScrollBar(Qt::Horizontal);
  vbLayout->addWidget(hScroll);

  setLayout(vbLayout);

  connect(audioController, SIGNAL(wavOpen(WavFile*)), this, SLOT(onWavOpen(WavFile*)));
  connect(audioController, SIGNAL(wavClose()), this, SLOT(onWavClose()));
  connect(audioController, SIGNAL(playPosition(qint64)), this, SLOT(onPlayPosition(qint64)));
}
/*----------------------------------------------------------------------------*/
EditPanel :: ~EditPanel()
{
}
/*----------------------------------------------------------------------------*/
void EditPanel :: newChannel()
{
  ChannelEdit *edit = new ChannelEdit(this);
  vbLayout->addWidget(edit);
  edits.append(edit);
  edit->setChannelId(edits.size());
  edit->setBgColor(palette().color(QPalette::Base));
  edit->setFgColor(palette().color(QPalette::Foreground));
  edit->setMarkerColor(Qt::darkBlue);

  connect(m_waveForm, SIGNAL(windowStartChanged(qint64)), edit, SLOT(setWindowStart(qint64)));
  connect(audioController, SIGNAL(wavOpen(WavFile*)), edit, SLOT(wavFileOpened(WavFile*)));
  connect(audioController, SIGNAL(wavClose()), edit, SLOT(wavFileClosed()));
  connect(audioController, SIGNAL(playPosition(qint64)), edit, SLOT(setFilePosition(qint64)));
  connect(parent()->parent()->parent(), SIGNAL(changeEditMode()), edit, SLOT(onChangeEditMode()));
}
/*----------------------------------------------------------------------------*/
void EditPanel :: onWavOpen(WavFile *)
{
  hScroll->setMaximum(2 * (int)(audioController->length() / m_waveForm->windowLength()));
  hScroll->setSingleStep(1);
  hScroll->setPageStep(10);
  connect(hScroll, SIGNAL(valueChanged(int)), this, SLOT(onScrollChanged(int)));
}
/*----------------------------------------------------------------------------*/
void EditPanel :: onScrollChanged(int pos)
{
  if(audioController->isPlay())
    return;
  qint64 p = (qint64)pos * m_waveForm->windowLength()/2;
  m_waveForm->setFilePosition(p);
  int size = edits.size();
  for(int i = 0; i < size; i++)
    edits[i]->setFilePosition(p);
}
/*----------------------------------------------------------------------------*/
void EditPanel :: onWavClose()
{
  hScroll->setMaximum(0);
}
/*----------------------------------------------------------------------------*/
void EditPanel :: onPlayPosition(qint64 samples)
{
  hScroll->setSliderPosition(2 * (int)(samples / m_waveForm->windowLength()));
}
/*----------------------------------------------------------------------------*/
void EditPanel :: onStartPlay()
{
}
/*----------------------------------------------------------------------------*/
void EditPanel :: onStopPlay()
{
}
/*----------------------------------------------------------------------------*/
void EditPanel :: onWindowStartChanged(qint64)
{
}
/*----------------------------------------------------------------------------*/
void EditPanel :: setWindowDuration(qint64 duration)
{
  m_waveForm->setWindowDuration(duration);
  int size = edits.size();
  for(int i = 0; i < size; i++)
    edits[i]->setWindowDuration(duration);
  if(m_waveForm->windowLength())
    hScroll->setMaximum(2 * (int)(audioController->length() / m_waveForm->windowLength()));
}
/*----------------------------------------------------------------------------*/
