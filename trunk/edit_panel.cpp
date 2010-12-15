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
  :Inherited(parent)
{
  vbLayout = new QVBoxLayout(this);
  vbLayout->setMargin(0);
  m_channelId = 1;

  m_waveForm = new WaveFormWidget(this);
  m_waveForm->setBgColor(palette().color(QPalette::Window));
  m_waveForm->setFgColor(palette().color(QPalette::Foreground));
  m_waveForm->setMarkerColor(Qt::darkBlue);
  m_waveForm->setGridColor(Qt::darkGray);
  m_waveForm->setMinimumHeight(100);
  m_waveForm->setMaximumHeight(200);



  connect(audioController, SIGNAL(wavOpen(WavFile*)), m_waveForm, SLOT(wavFileOpened(WavFile*)));
  connect(audioController, SIGNAL(wavClose()), m_waveForm, SLOT(wavFileClosed()));
  connect(audioController, SIGNAL(playPosition(qint64)), m_waveForm, SLOT(setFilePosition(qint64)));

  m_channelEdit = new ChannelEdit(this);
  vbLayout->addWidget(m_channelEdit);
  m_channelEdit->setChannelId(m_channelId);
  m_channelEdit->setBgColor(palette().color(QPalette::Base));
  m_channelEdit->setFgColor(palette().color(QPalette::Foreground));
  m_channelEdit->setMarkerColor(Qt::darkBlue);

  connect(m_waveForm, SIGNAL(windowStartChanged(qint64)), m_channelEdit, SLOT(setWindowStart(qint64)));
  connect(audioController, SIGNAL(wavOpen(WavFile*)), m_channelEdit, SLOT(wavFileOpened(WavFile*)));
  connect(audioController, SIGNAL(wavClose()), m_channelEdit, SLOT(wavFileClosed()));
  connect(audioController, SIGNAL(playPosition(qint64)), m_channelEdit, SLOT(setFilePosition(qint64)));
  connect(effectController, SIGNAL(modeChange()), m_channelEdit, SLOT(onChangeEditMode()));
  connect(effectController, SIGNAL(efectSelect(int)), m_channelEdit, SLOT(onSelectionChanged()));

  vbLayout->addWidget(m_waveForm);

  hScroll = new QScrollBar(Qt::Horizontal);

  vbLayout->addWidget(hScroll);
  vbLayout->setSpacing(1);

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
void EditPanel :: onWavOpen(WavFile *)
{
  hScroll->setMaximum((int)(audioController->length() / m_waveForm->windowLength()));
  hScroll->setSingleStep(1);
  hScroll->setPageStep(10);
  connect(hScroll, SIGNAL(valueChanged(int)), this, SLOT(onScrollChanged(int)));
}
/*----------------------------------------------------------------------------*/
void EditPanel :: onScrollChanged(int pos)
{
  if(audioController->isPlay())
    return;
  qint64 p = (qint64)pos * m_waveForm->windowLength();
  m_waveForm->setFilePosition(p);
  m_channelEdit->setFilePosition(p);
}
/*----------------------------------------------------------------------------*/
void EditPanel :: onWavClose()
{
  hScroll->setMaximum(0);
}
/*----------------------------------------------------------------------------*/
void EditPanel :: onPlayPosition(qint64 samples)
{
  hScroll->setSliderPosition( (int)(samples / m_waveForm->windowLength()));
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
  m_channelEdit->setWindowDuration(duration);
  if(m_waveForm->windowLength())
    hScroll->setMaximum((int)(audioController->length() / m_waveForm->windowLength()));
}
/*----------------------------------------------------------------------------*/
void EditPanel :: setChannel(int c)
{
  m_channelEdit->setChannelId(c);
}
/*----------------------------------------------------------------------------*/
