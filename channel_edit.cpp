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
static ChannelEdit::Mode m_mode = ChannelEdit::SelectMode;
static int m_effectToAdd = 0;

ChannelEdit :: ChannelEdit(QWidget *parent)
  :Inherited(parent)
{
  setFixedHeight(40);
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

    QBrush brush(Qt::yellow);
    painter.setBrush(brush);

    int size = effects.size();
    for(int i = 0; i < size; i++)
    {
      EffectProperties *prop = effects[i];
      int pos = (int)(dX * (audioController->duration2quants(prop->timeStart()) - m_startPosition));
      painter.drawLine(QPoint(pos, 0), QPoint(pos, height()));
      if(prop->channel() == m_channelId)
      {
        //draw effect
        QPolygon pol = createPolygon(pos);
        painter.drawPolygon(pol);
        painter.drawPixmap(QRect(pos - 15, 1, 30, 30), effectController->effectIcon(prop->effectId()));
/*
        qreal radius = (height() - 10) / 2;
        qreal left = pos - radius;
        qreal top = 5;

        QRectF r(left, top, 2 * radius, 2 * radius);
        painter.drawEllipse(r);
*/
      }
    }
  }

  Inherited::paintEvent(event);
}
/*----------------------------------------------------------------------------*/
QPolygon ChannelEdit :: createPolygon(int x)
{
  QPolygon pol;
  const int w = 32;
  const int h = 32;
  pol.append(QPoint(x-w/2, 0));
  pol.append(QPoint(x+w/2, 0));
  pol.append(QPoint(x+w/2, h));
  pol.append(QPoint(x+3, rect().bottom() - 5));
  pol.append(QPoint(x+2, rect().bottom() - 3));
  pol.append(QPoint(x+1, rect().bottom() - 1));
  pol.append(QPoint(x, rect().bottom()));
  pol.append(QPoint(x-1, rect().bottom() - 1));
  pol.append(QPoint(x-2, rect().bottom() - 3));
  pol.append(QPoint(x-3, rect().bottom() - 5));
  pol.append(QPoint(x-w/2, h));
  return pol;
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
  update();
}
/*----------------------------------------------------------------------------*/
void ChannelEdit :: updateData()
{
  if(m_audioOpened)
  {
    m_windowSize = audioController->duration2quants(m_windowDurationUs);
    qint64 start = audioController->quants2duration(m_startPosition);
    effects = effectController->selectEffects(start, m_windowDurationUs);
    setMouseTracking(true);
    update();
  }
  else
    setMouseTracking(false);

}
/*----------------------------------------------------------------------------*/
void ChannelEdit :: wavFileOpened(WavFile *w)
{
  m_audioOpened = true;
  updateData();
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
  if(m_windowDurationUs == duration)
    return;
  m_windowDurationUs = duration;
  updateData();
}
/*----------------------------------------------------------------------------*/
void ChannelEdit :: setWindowStart(qint64 quants)
{
  if(m_startPosition == quants)
    return;
  m_startPosition = quants;
  updateData();
}
/*----------------------------------------------------------------------------*/
void ChannelEdit :: setChannelId(int i)
{
  m_channelId = i;
  if(m_audioOpened)
    update();
}
/*----------------------------------------------------------------------------*/
void ChannelEdit :: onChangeEditMode()
{
  switch(m_mode)
  {
  case SelectMode:
    setCursor(QCursor(Qt::ArrowCursor));
    break;
  case AddMode:
    setCursor(QCursor(effectController->effectIcon(m_effectToAdd).scaledToWidth(16)));
  }
}
/*----------------------------------------------------------------------------*/
ChannelEdit::Mode ChannelEdit :: mode()
{
  return m_mode;
}
/*----------------------------------------------------------------------------*/
void ChannelEdit :: setMode(ChannelEdit::Mode m)
{
  m_mode = m;
}
/*----------------------------------------------------------------------------*/
int ChannelEdit :: effectToAdd()
{
  return m_effectToAdd;
}
/*----------------------------------------------------------------------------*/
void ChannelEdit :: setEffectToAdd(int effectId)
{
  m_effectToAdd = effectId;
}
/*----------------------------------------------------------------------------*/
