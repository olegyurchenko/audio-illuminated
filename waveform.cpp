/*----------------------------------------------------------------------------*/
/**
* @pkg waveform
*/
/**
* Waveform widget.
*
* Long description of waveform.<br>
* (C) T&T, Kiev, Ukraine 2010.<br>
* started 07.12.2010 13:07:57<br>
* @pkgdoc waveform
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "waveform.h"
#include <wav_file.h>
#include <QPainter>
#include <QFile>
#include <QtDebug>
#include <QResizeEvent>
#include <QRect>
#include <QImage>

WaveFormWidget :: WaveFormWidget(QWidget *parent)
  :QWidget(parent), m_pixmap(QSize(0,0))
{
  m_startPosition = 0;
  m_windowDurationUs = 1000000; //1s
  m_timeRuler = 100000; //100ms
  m_windowSize = 0;
  m_position = 0;

  m_selectionStart = 0;
  m_selectionLength = 0;


  m_wav = NULL;
  m_in = NULL;

  m_bgColor = Qt::black;
  m_fgColor = Qt::white;
  m_gridColor = Qt::yellow;
  m_markerColor = Qt::green;
}
/*----------------------------------------------------------------------------*/
WaveFormWidget :: ~WaveFormWidget()
{
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: setWavFile(WavFile *w)
{
  m_wav = w;
  if(m_in != NULL)
    delete m_in;
  m_in = NULL;
  if(m_wav == NULL)
  {
    m_pixmap = QPixmap(QSize(0,0));
    m_windowSize = 0;
    return;
  }

  m_windowSize = m_wav->length(m_windowDurationUs);
  m_startPosition = 0;
  m_in = new QFile(m_wav->fileName());
  if(!m_in->open(QIODevice::ReadOnly))
  {
    delete m_in;
    m_in = NULL;
    m_wav = NULL;
    return;
  }
  setFilePosition(0);
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: wavFileOpened(WavFile *w)
{
  setWavFile(w);
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: wavFileClosed()
{
  setWavFile(NULL);
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: paintEvent(QPaintEvent *event)
{
  QPainter painter(this);

  painter.fillRect(rect(), m_bgColor);
  painter.drawPixmap(0, 0, m_pixmap);
  if(m_windowSize)
  {
    qreal dX = (qreal)width() / (qreal) m_windowSize;
    QPen pen(m_markerColor);
    painter.setPen(pen);
    //Draw position line
    painter.drawLine(QPointF(dX * (m_position - m_startPosition), 0), QPointF(dX * (m_position - m_startPosition), height()));
    //Draw user selection
    if(
        (m_selectionStart >= m_startPosition && m_selectionStart < m_startPosition + m_windowSize)
        || (m_selectionStart + m_selectionLength >= m_startPosition && m_selectionStart + m_selectionLength < m_startPosition + m_windowSize)
        || (m_selectionStart < m_startPosition && m_selectionStart + m_selectionLength > m_startPosition + m_windowSize)
      )
    {
      QRect r = rect();
      if(m_selectionStart > m_startPosition)
        r.setLeft((int)(dX * (m_selectionStart - m_startPosition)));
      if(m_selectionStart + m_selectionLength < m_startPosition + m_windowSize)
        r.setRight(1+(int)(dX * (m_selectionStart + m_selectionLength - m_startPosition)));

      QImage img(r.size(), QImage::Format_RGB32);
      QPainter painter1(&img);
      painter1.drawPixmap(QPoint(0, 0), m_pixmap, r);
      img.invertPixels();
      painter.drawImage(r.left(), r.top(), img);

      /*
      QPixmap sel = m_pixmap.copy(r);
      QPainter painter1(&sel);

      painter1.setCompositionMode(QPainter::CompositionMode_Xor);
      painter1.fillRect(sel.rect(), Qt::black);
      painter.drawPixmap(r.left(), r.top(), sel);
      */
      /*
      QBrush brush(Qt::white, Qt::Dense5Pattern);
      painter.fillRect(r, brush);
      */
    }

  }
  //QWidget::paintEvent(event);
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: setStartSelection(int x)
{
  bool modified = false;
  qint64 pos = pixel2audio(x);
  modified = m_selectionStart != pos;
  m_selectionStart = pos;
  m_selectionLength = 0;

  if(modified)
    update();
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: setEndSelection(int x)
{
  bool modified = false;
  qint64 selectionStart = m_selectionStart;
  qint64 selectionLength = m_selectionLength;
  qint64 pos = pixel2audio(x);
/*
  qint64 l = qMin<qint64>(pos, selectionStart);
  qint64 r = qMax<qint64>(pos, selectionStart + selectionLength);
*/
  qint64 l;
  qint64 r;
  if(pos < selectionStart)
  {
    l = pos;
    r = selectionStart + selectionLength;
  }
  else
  {
    l = selectionStart;
    r = pos;
  }

  selectionStart = l;
  selectionLength = r - l;

  modified = m_selectionStart != selectionStart || m_selectionLength != selectionLength;
  m_selectionStart = selectionStart;
  m_selectionLength = selectionLength;

  if(modified)
    update();

}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: mousePressEvent(QMouseEvent *event)
{
  if(event->modifiers() & (Qt::ShiftModifier | Qt::ControlModifier))
  {
    setEndSelection(event->pos().x());
  }
  else
  {
    setStartSelection(event->pos().x());
  }
  QWidget::mousePressEvent(event);
  //qDebug() << "mousePressEvent" << m_selectionStart << m_selectionLength;
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: mouseReleaseEvent(QMouseEvent * event)
{
  QWidget::mouseReleaseEvent(event);
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: mouseMoveEvent (QMouseEvent * event)
{
  setEndSelection(event->pos().x());
  QWidget::mouseMoveEvent(event);
  //qDebug() << "mouseMoveEvent" << m_selectionStart << m_selectionLength;
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: resizeEvent(QResizeEvent *event)
{
  drawPixmap(event->size());
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: setFilePosition(qint64 pos)
{
  if(m_wav == NULL || m_in == NULL)
    return;

  bool modified = m_position != pos;
  m_position = pos;
  if(modified && (m_position > m_startPosition + m_windowSize || m_position < m_startPosition) || m_pixmap.size() != size())
  {
    if(m_position > m_startPosition + m_windowSize || m_position < m_startPosition)
    {
      m_startPosition = m_windowSize * (m_position / m_windowSize);
      emit windowStartChanged(m_startPosition);
    }

    modified = true;
    drawPixmap(size());
  }
  if(modified)
    update();
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: drawPixmap(const QSize& size)
{
  if(m_wav == NULL || m_in == NULL)
  {
    m_pixmap = QPixmap(QSize(0,0));
    return;
  }

  qint64 timeRuler = m_wav->length(m_timeRuler);

  if(m_pixmap.size() != size)
    m_pixmap = QPixmap(size);

  QPainter painter(&m_pixmap);

  int channels = m_wav->format().channels();

  QPen pen(m_fgColor);
  painter.setPen(pen);

  painter.fillRect(m_pixmap.rect(), m_fgColor);

  qreal dX = (qreal)m_pixmap.width() / (qreal) m_windowSize;
  qreal dY = (qreal)m_pixmap.height() / (2 * (qreal) channels);

  QVector<QLine> lines(channels);
  QVector<int> baseY(channels);
  //painter.drawLine(QPoint(0,100), QPoint(m_pixmap.width(), 100));
  QPen pen1(m_gridColor);
  painter.setPen(pen1);
  for(int i = 0; i < channels; i++)
  {
    baseY[i] = (int)(dY + dY * i * 2);
    lines[i].setP1(QPoint(0, baseY[i]));
    lines[i].setP2(QPoint(m_pixmap.width(), baseY[i]));
    painter.drawLine(lines[i]);
  }

  //Draw ruler
#if 1
  painter.setPen(pen1);
  for(qint64 p = 0; timeRuler && p < m_windowSize; p++)
    if(!((m_startPosition + p) % timeRuler))
    {
      qreal x = p * dX;
      painter.drawLine((int)x, 0, (int)x, m_pixmap.height());
    }
#endif

  //Draw signal
  painter.setPen(pen);
  m_wav->seek(m_startPosition, *m_in);
  for(qint64 p = 0; p < m_windowSize; p++)
  {
    qreal x = p * dX;
    QVector<qreal> data = m_wav->readNext(*m_in);
    int c = data.size();
    for(int i = 0; i < channels && i < c; i++)
    {
      qreal y = data[i] * dY + baseY[i];
      lines[i].setP2(QPoint((int)x, (int)y));
      painter.drawLine(lines[i]);
      lines[i].setP1(QPoint((int)x, (int)y));
    }
  }
}
/*----------------------------------------------------------------------------*/
qint64 WaveFormWidget :: pixel2audio(int x) const
{
  if(!m_windowSize || !width())
    return 0;

  qreal dX = (qreal) m_windowSize/(qreal)width();
  return (qint64)((qreal)x * dX) + m_startPosition;
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: setBgColor(QColor c)
{
  if(m_bgColor == c)
    return;
  m_bgColor = c;
  drawPixmap(size());
  update();
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: setFgColor(QColor c)
{
  if(m_fgColor == c)
    return;
  m_fgColor = c;
  drawPixmap(size());
  update();
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: setGridColor(QColor c)
{
  if(m_gridColor == c)
    return;
  m_gridColor = c;
  drawPixmap(size());
  update();
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: setMarkerColor(QColor c)
{
  if(m_markerColor == c)
    return;
  m_markerColor = c;
  update();
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: setWindowDuration(qint64 duration)
{
  if(m_windowDurationUs == duration)
    return;
  m_windowDurationUs = duration;
  if(m_wav != NULL)
  {
    m_windowSize = m_wav->length(m_windowDurationUs);
    drawPixmap(size());
    update();
  }
}
/*----------------------------------------------------------------------------*/
