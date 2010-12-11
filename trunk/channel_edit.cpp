/*----------------------------------------------------------------------------*/
/**
* @pkg channel_edit
*/
/**
* Widget to edit illumination channel.
*
* Copyright (C) 2010 Oleg Yurchenko, Kiev, Ukraine.<br>
* started 10.12.2010 10:23:23<br>
* @pkgdoc channel_edit
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "channel_edit.h"
#include <QPainter>
#include <QRectF>
#include <audio_controller.h>
/*----------------------------------------------------------------------------*/
ChannelEdit :: ChannelEdit(QWidget *parent)
  :Inherited(parent)
{
  setFixedHeight(50);
  m_audioOpened = false;
  m_startPosition = 0;
  m_windowDurationUs = 1000000;
  m_timeRuler = 100000;
  m_windowSize = 0;
  m_position = 0;

  m_bgColor = Qt::white;
  m_fgColor = Qt::black;
  m_gridColor = Qt::yellow;
  m_markerColor = Qt::green;
}
/*----------------------------------------------------------------------------*/
ChannelEdit :: ~ChannelEdit()
{
}
/*----------------------------------------------------------------------------*/
void ChannelEdit :: paintEvent(QPaintEvent *event)
{
  QPainter painter(this);

  painter.fillRect(rect(), m_bgColor);
  if(m_audioOpened)
  {
    qreal dX = (qreal)width() / (qreal) m_windowSize;

    QPen pen(m_markerColor);
    painter.setPen(pen);
    //Draw position line
    painter.drawLine(QPointF(dX * (m_position - m_startPosition), 0), QPointF(dX * (m_position - m_startPosition), height()));

    QPen pen1(m_fgColor);
    painter.setPen(pen1);

    int size = effects.size();
    for(int i = 0; i < size; i++)
    {
      EffectProperties *prop = effects[i];
      if(prop->channel() == m_channelId)
      {
        //draw effect
        qreal pos = dX * (audioController->duration2quants(prop->timeStart()) - m_startPosition);

        painter.drawLine(QPointF(pos, 0), QPointF(pos, height()));


        qreal radius = (height() - 10) / 2;
        qreal left = pos - radius;
        qreal top = 10;

        QRectF r(left, top, 2 * radius, 2 * radius);
        painter.drawEllipse(r);



      }
    }
  }

  Inherited::paintEvent(event);
}
/*----------------------------------------------------------------------------*/
void ChannelEdit :: resizeEvent(QResizeEvent *event)
{
  Inherited::resizeEvent(event);
}
/*----------------------------------------------------------------------------*/
void ChannelEdit :: mousePressEvent(QMouseEvent *event)
{
  Inherited::mousePressEvent(event);
}
/*----------------------------------------------------------------------------*/
void ChannelEdit :: mouseReleaseEvent(QMouseEvent * event)
{
  Inherited::mouseReleaseEvent(event);
}
/*----------------------------------------------------------------------------*/
void ChannelEdit :: mouseMoveEvent (QMouseEvent * event)
{
  Inherited::mouseMoveEvent(event);
}
/*----------------------------------------------------------------------------*/
void ChannelEdit :: setFilePosition(qint64 position)
{
  m_position = position;
}
/*----------------------------------------------------------------------------*/
void ChannelEdit :: wavFileOpened(WavFile *w)
{
  m_audioOpened = true;
  m_windowSize = audioController->duration2quants(m_windowDurationUs);
  qint64 start = audioController->quants2duration(m_startPosition);
  effects = effectController->selectEffects(start, m_windowDurationUs);
  update();
}
/*----------------------------------------------------------------------------*/
void ChannelEdit :: wavFileClosed()
{
  m_audioOpened = false;
  m_windowSize = 0;
  effects.clear();
  update();
}
/*----------------------------------------------------------------------------*/
void ChannelEdit :: setBgColor(QColor c)
{
  m_bgColor = c;
  update();
}
/*----------------------------------------------------------------------------*/
void ChannelEdit :: setFgColor(QColor c)
{
  m_fgColor = c;
  update();
}
/*----------------------------------------------------------------------------*/
void ChannelEdit :: setGridColor(QColor c)
{
  m_gridColor = c;
  update();
}
/*----------------------------------------------------------------------------*/
void ChannelEdit :: setMarkerColor(QColor c)
{
  m_markerColor = c;
  update();
}
/*----------------------------------------------------------------------------*/
void ChannelEdit :: setWindowDuration(qint64 duration)
{
  if(m_windowDuration == duration)
    return;
  m_windowDuration = duration;
  if(m_audioOpened)
  {
    m_windowSize = audioController->duration2quants(duration);
    qint64 start = audioController->quants2duration(m_startPosition);
    effects = effectController->selectEffects(start, m_windowDurationUs);
    update();
  }
}
/*----------------------------------------------------------------------------*/
void ChannelEdit :: setWindowStart(qint64 quants)
{
  if(m_startPosition == quants)
    return;
  m_startPosition = quants;
  if(m_audioOpened)
  {
    qint64 start = audioController->quants2duration(m_startPosition);
    effects = effectController->selectEffects(start, m_windowDurationUs);
    update();
  }
}
/*----------------------------------------------------------------------------*/
void ChannelEdit :: setChannelId(int i)
{
  m_channelId = i;
  if(m_audioOpened)
    update();
}
/*----------------------------------------------------------------------------*/

