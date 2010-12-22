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
#include <QPoint>

#define ABS(a) ((a) < 0 ? -(a) : (a))

/*----------------------------------------------------------------------------*/
WaveFormThread :: WaveFormThread(WaveFormWidget *parent)
  : QThread(parent)
{
  m_waveform = parent;
  m_terminated = false;
}
/*----------------------------------------------------------------------------*/
WaveFormThread :: ~WaveFormThread()
{
  m_terminated = true;
  clear();
  m_sem.release();
  wait();
}
/*----------------------------------------------------------------------------*/
int WaveFormThread :: transNo()
{
  //m_mut.lock();
  int n = m_transNo;
  //m_mut.unlock();
  return n;
}
/*----------------------------------------------------------------------------*/
void WaveFormThread :: run()
{
  while(!m_terminated)
  {
    if(!m_sem.tryAcquire(1, 100))
      continue;

    bool empty = false;
    WaveFormWidget::Tile tile;
    int transNo;

    lock();
    m_mut.lock();
    empty = m_tiles.isEmpty();
    if(!empty)
    {
      tile = m_tiles.begin().value();
      m_tiles.erase(m_tiles.begin());
    }
    transNo = m_transNo;
    m_mut.unlock();
    if(empty)
    {
      unlock();
      continue;
    }

    m_waveform->drawTile(tile);
    unlock();
    emit tilePainted(tile.index, transNo);

    m_mut.lock();
    empty = m_tiles.isEmpty();
    m_mut.unlock();
    if(empty)
      emit ready();
  }
}
/*----------------------------------------------------------------------------*/
void WaveFormThread :: addTile(const WaveFormWidget::Tile& tile)
{
  m_mut.lock();
  m_tiles[tile.index] = tile;
  m_mut.unlock();
  m_sem.release();
}
/*----------------------------------------------------------------------------*/
void WaveFormThread :: clearQueue()
{
  m_mut.lock();
  m_tiles.clear();
  m_transNo ++;
  m_mut.unlock();
}
/*----------------------------------------------------------------------------*/
void WaveFormThread :: clear()
{
  lock();
  m_tiles.clear();
  m_transNo ++;
  unlock();
}
/*----------------------------------------------------------------------------*/
WaveFormWidget :: WaveFormWidget(QWidget *parent)
  :QWidget(parent)
{
  m_startPosition = 0;
  m_windowDurationUs = 1000000; //1s
  m_timeRuler = 100000; //100ms
  m_windowSize = 0;
  m_position = 0;

  m_selectionStart = 0;
  m_selectionLength = 0;

  m_tileDuration = 20000; //20ms

  m_wav = NULL;
  m_in = NULL;

  m_bgColor = Qt::black;
  m_fgColor = Qt::white;
  m_gridColor = Qt::yellow;
  m_markerColor = Qt::green;
  m_noPainted = 0;
  m_thread = new WaveFormThread(this);
  connect(m_thread, SIGNAL(tilePainted(int,int)), this, SLOT(onTilePainted(int,int)), Qt::QueuedConnection);
  connect(m_thread, SIGNAL(ready()), this, SLOT(onTilesReady()), Qt::QueuedConnection);
  m_thread->start();
}
/*----------------------------------------------------------------------------*/
WaveFormWidget :: ~WaveFormWidget()
{
  delete m_thread;
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: clear()
{
  m_thread->clear();
  int size = m_tiles.size();
  for(int i = 0; i < size; i++)
    if(m_tiles[i].image != NULL)
      delete m_tiles[i].image;
  m_tiles.clear();
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: init(QSize size)
{
  clear();
  m_windowSize = m_wav->length(m_windowDurationUs);
  m_tileSize = m_wav->length(m_tileDuration);
  m_tileCount = m_windowSize / m_tileSize;
  /*
  if(m_tileCount > 100)
  {
    m_tileSize = m_windowSize / 100;
    m_tileCount = 100;
  }
  */
  QSize tileSize(1 + size.width()/m_tileCount, size.height());
  //QSize tileSize(m_tileSize, m_wav->format().channels() * 200);
  for(qint64 i = 0; i < m_tileCount; i++)
  {
    Tile t;
    t.index = (int) i;
    t.startPosition = i * m_tileSize + m_startPosition;
    t.image = new QImage(tileSize, QImage::Format_RGB32);
    t.painted = false;
    m_tiles.append(t);
  }
  m_noPainted = 0;
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: drawTiles()
{
  m_thread->clearQueue();
  m_noPainted = 0;
  int size = m_tiles.size();
  for(int i = 0; i < size; i++)
  {
    if(!m_tiles[i].painted)
    {
      m_tiles[i].index = i;
      m_tiles[i].startPosition = m_tileSize * i + m_startPosition;
      m_thread->addTile(m_tiles[i]);
      m_noPainted ++;
    }
  }
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: redrawTiles()
{
  m_thread->clearQueue();
  m_noPainted = 0;
  int size = m_tiles.size();
  for(int i = 0; i < size; i++)
  {
    m_tiles[i].painted = false;
    m_tiles[i].index = i;
    m_tiles[i].startPosition = m_tileSize * i + m_startPosition;
    m_thread->addTile(m_tiles[i]);
    m_noPainted ++;
  }
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: drawTile(Tile &tile)
{
  QImage *image = tile.image;
  if(image == NULL)
    return;

  QPainter painter(image);

  int channels = m_wav->format().channels();

  QPen pen(m_fgColor);
  painter.setPen(pen);

  painter.fillRect(image->rect(), m_bgColor);

  qreal dX = (qreal)image->width() / (qreal) m_tileSize;
  qreal dY = (qreal)image->height() / (2 * (qreal) channels);

  typedef QVector<QPoint> Points;
  QVector<Points> points(channels, Points(m_tileSize));

  QVector<int> baseY(channels);

  QPen pen1(m_gridColor);
  painter.setPen(pen1);
  for(int i = 0; i < channels; i++)
  {
    QLine line;
    baseY[i] = (int)(dY + dY * i * 2);
    QPoint p1(0, baseY[i]);
    line.setP1(p1);
    line.setP2(QPoint(image->width(), baseY[i]));
    painter.drawLine(line);
    points[i][0] = p1;
  }


  //Draw signal
  if(tile.startPosition) //read last point
  {
    m_wav->seek(tile.startPosition - 1, *m_in);
    QVector<qreal> data = m_wav->readNext(*m_in);
    int c = data.size();
    for(int i = 0; i < channels && i < c; i++)
    {
      qreal y = data[i] * dY + baseY[i];
      points[i][0] = QPoint(0, (int)y);
    }
  }
  else
    m_wav->seek(tile.startPosition, *m_in);
  for(qint64 p = 0; p < m_tileSize; p++)
  {
    qreal x = p * dX;
    QVector<qreal> data = m_wav->readNext(*m_in);
    int c = data.size();
    for(int i = 0; i < channels && i < c; i++)
    {
      qreal y = data[i] * dY + baseY[i];
      points[i][p] = QPoint((int)x, (int)y);
    }
  }

  painter.setPen(pen);
  for(int i = 0; i < channels; i++)
  {
    Points &p = points[i];
    painter.drawPolyline(p.data(), p.size());
  }

  tile.painted = true;
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: setWavFile(WavFile *w)
{
  m_thread->clear();
  m_wav = w;
  if(m_in != NULL)
    delete m_in;
  m_in = NULL;
  if(m_wav == NULL)
  {
    clear();
    m_windowSize = 0;
    return;
  }

  m_startPosition = 0;
  m_in = new QFile(m_wav->fileName());
  if(!m_in->open(QIODevice::ReadOnly))
  {
    delete m_in;
    m_in = NULL;
    m_wav = NULL;
    return;
  }
  init(size());
  redrawTiles();
  update();
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
  if(!m_windowSize || m_wav == NULL || m_tiles.empty())
    return;

  qreal dX = (qreal)width() / (qreal) m_windowSize;

//  painter.drawPixmap(0, 0, m_pixmap);
  int size = m_tiles.size();
  for(int i = 0; i < size; i++)
  {
    if(m_tiles[i].painted)
      painter.drawImage((int)(i * m_tileSize * dX), 0, *m_tiles[i].image);
  }

  QPen pen(m_markerColor);
  QPen pen1(m_fgColor);
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
    painter1.fillRect(QRect(0,0,r.width(), r.height()), m_bgColor);
    for(int i = 0; i < size; i++)
    {
      if(!m_tiles[i].painted)
        continue;
      int x = (int)(i * m_tileSize * dX);
      if(x < r.right() && x + m_tiles[i].image->width() >= r.left())
        painter1.drawImage(QPoint(x - r.left(), 0), *m_tiles[i].image);
    }

    painter1.setPen(pen);
    //Draw position line
    painter1.drawLine(QPointF( dX * (m_position - m_startPosition) - r.left(), 0), QPointF( dX * (m_position - m_startPosition) - r.left(), img.height()));
    img.invertPixels();

    painter.drawImage(r.left(), r.top(), img);
    painter.setPen(pen1);
    painter.drawRect(r);

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
  if(!m_windowSize || m_wav == NULL)
    return;
  init(event->size());
  drawTiles();
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: setFilePosition(qint64 pos)
{
  if(m_wav == NULL || m_in == NULL)
    return;

  m_position = pos;
  qint64 wavSize = m_wav->length();
  qint64 newStart = m_startPosition;

  if(m_position < m_windowSize/2)
    newStart = 0;
  else
  if(m_position > wavSize - m_windowSize/2)
    newStart = m_tileSize * ((wavSize - m_windowSize)/m_tileSize); //m_tileSize adjusted
  else
    newStart = m_tileSize * ((m_position - m_windowSize/2)/m_tileSize); //m_tileSize adjusted

  if(newStart == m_startPosition)
  {
    drawTiles();
    update();
    return;
  }


  if(ABS(m_startPosition - newStart) >= m_windowSize)
  {
    m_startPosition = newStart;
    int size = m_tiles.size();
    for(int i = 0; i < size; i++)
      m_tiles[i].painted = false;
  }
  else
  {
    while(newStart < m_startPosition) //scrool left
    {
      m_tiles.last().painted = false;
      m_tiles.push_front(m_tiles.last());
      m_tiles.pop_back();
      m_startPosition -= m_tileSize;
    }

    while(newStart > m_startPosition) //scrool right
    {
      m_tiles.front().painted = false;
      m_tiles.push_back(m_tiles.front());
      m_tiles.pop_front();
      m_startPosition += m_tileSize;
    }
  }

  drawTiles();
  update();
  emit windowStartChanged(m_startPosition);
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
  redrawTiles();
  update();
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: setFgColor(QColor c)
{
  if(m_fgColor == c)
    return;
  m_fgColor = c;
  redrawTiles();
  update();
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: setGridColor(QColor c)
{
  if(m_gridColor == c)
    return;
  m_gridColor = c;
  redrawTiles();
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
    init(size());
    drawTiles();
    update();
  }
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: onTilePainted(int index, int transNo)
{
  if(index >= 0 && index <= m_tiles.size() && transNo == m_thread->transNo())
  {
    //m_tiles[index].image = image;
    m_tiles[index].painted = true;
  }
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: onTilesReady()
{
  update();
}
/*----------------------------------------------------------------------------*/
