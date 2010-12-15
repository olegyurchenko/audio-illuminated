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
#include <QMouseEvent>
#include <QKeyEvent>
/*----------------------------------------------------------------------------*/

ChannelEdit :: ChannelEdit(QWidget *parent)
  :Inherited(parent)
{
  setFixedHeight(40);
  setFocusPolicy( Qt::StrongFocus);
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
  m_selected = -1;
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
  painter.setPen(Qt::darkCyan);
  painter.drawText(QRectF(0,0,50, height()),
                   Qt::AlignCenter,
                   tr("Channel\n%1").arg(QString::number(m_channelId)));
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
      int pos = (int)(dX * (audioController->duration2quants(prop->timeStart()) - m_startPosition));
      painter.drawLine(QPoint(pos, 0), QPoint(pos, height()));
      if(prop->channel() < m_channelId)
      {
        //draw bottom arrow
        painter.drawLine(QPoint(pos - 2, height() - 8), QPoint(pos, height()));
        painter.drawLine(QPoint(pos + 2, height() - 8), QPoint(pos, height()));
      }
      else
      if(prop->channel() > m_channelId)
      {
        //draw top arrow
        painter.drawLine(QPoint(pos - 2, 8), QPoint(pos, 0));
        painter.drawLine(QPoint(pos + 2, 8), QPoint(pos, 0));
      }
      else
//      if(prop->channel() == m_channelId)
      {
        //draw effect
        if(effectController->effectSelected() == prop->id())
        {
          QBrush brush(Qt::darkBlue);
          painter.setBrush(brush);
        }
        else
        {
          QBrush brush(Qt::yellow);
          painter.setBrush(brush);
        }

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
  if(m_audioOpened)
  {
    qreal dX = (qreal)width() / (qreal) m_windowSize;
    if(effectController->mode() == EffectController::SelectMode)
    {
      int size = effects.size();
      int i;
      for(i = 0; i < size; i++)
      {
        EffectProperties *prop = effects[i];
        if(prop->channel() == m_channelId)
        {
          int pos = (int)(dX * (audioController->duration2quants(prop->timeStart()) - m_startPosition));
          QPolygon pol = createPolygon(pos);
          if(pol.containsPoint(event->pos(),Qt::OddEvenFill))
          {
            effectController->selectEffect(prop->id());
            m_selected = prop->id();
            break;
          }
        }
      }
      if(i >= size)
      {
        effectController->selectEffect(-1);
        m_selected = -1;
      }
    }
    else
    if(effectController->mode() == EffectController::AddMode)
    {
      qreal d =  (qreal) m_windowSize/ (qreal)width();
      EffectProperties *prop = effectController->newEffect(effectController->effectToAdd(), m_channelId);
      prop->setTimeStart(audioController->quants2duration(m_startPosition + (qint64)(event->pos().x() * d)));
      effectController->selectEffect(prop->id());
      effectController->setMode(EffectController::SelectMode);
    }
  }

  Inherited::mousePressEvent(event);
}
/*----------------------------------------------------------------------------*/
void ChannelEdit :: mouseReleaseEvent(QMouseEvent * event)
{
  m_selected = -1;
  Inherited::mouseReleaseEvent(event);
}
/*----------------------------------------------------------------------------*/
void ChannelEdit :: mouseMoveEvent (QMouseEvent * event)
{
  if(m_selected > 0)
  {
    qreal d =  (qreal) m_windowSize/ (qreal)width();
    EffectProperties *prop = effectController->findEffectProp(m_selected);
    prop->setTimeStart(audioController->quants2duration(m_startPosition + (qint64)(event->pos().x() * d)));
    effectController->selectEffect(prop->id());
  }

  Inherited::mouseMoveEvent(event);
}
/*----------------------------------------------------------------------------*/
void ChannelEdit :: keyPressEvent(QKeyEvent *event)
{
  if(m_audioOpened)
  {
    switch(event->key())
    {
    case Qt::Key_Delete:
      if(effectController->effectSelected() > 0)
        effectController->deleteEffect(effectController->effectSelected());
      break;
/*
    case Qt::Key_Left:
    case Qt::Key_Right:
*/
    }
  }
  Inherited::keyPressEvent(event);
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
    //setMouseTracking(true);
    update();
  }
/*
  else
    setMouseTracking(false);
*/
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
 if(m_channelId == i)
   return;
 m_channelId = i;
 effectController->selectEffect(-1);
 update();
}
/*----------------------------------------------------------------------------*/
void ChannelEdit :: onChangeEditMode()
{
  switch(effectController->mode())
  {
  case EffectController::SelectMode:
    setCursor(QCursor(Qt::ArrowCursor));
    break;
  case EffectController::AddMode:
    setCursor(QCursor(effectController->effectIcon(effectController->effectToAdd()).scaledToWidth(16)));
  }
}
/*----------------------------------------------------------------------------*/
void ChannelEdit :: onSelectionChanged()
{
  updateData();
  update();
}
/*----------------------------------------------------------------------------*/
