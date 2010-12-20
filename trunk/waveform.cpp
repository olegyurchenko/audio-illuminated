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
  wait(1000);
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

    m_mut.lock();
    empty = m_queue.isEmpty();
    if(!empty)
    {
      tile = m_queue.first();
      m_queue.pop_front();
    }
    m_mut.unlock();
    if(empty)
      continue;
    m_waveform->drawTile(tile);
    emit tilePainted(tile.index, tile.pixmap);
  }
}
/*----------------------------------------------------------------------------*/
void WaveFormThread :: addTile(const WaveFormWidget::Tile& tile)
{
  if(tile.pixmap == NULL)
    return;
  m_mut.lock();
  m_queue.push_back(tile);
  m_mut.unlock();
  m_sem.release();
}
/*----------------------------------------------------------------------------*/
void WaveFormThread :: clear()
{
  m_mut.lock();
  int size = m_queue.size();
  for(int i = 0; i < size; i++)
    delete m_queue[i].pixmap;
  m_queue.clear();
  m_mut.unlock();
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
  m_painted = false;
  m_thread = new WaveFormThread(this);
  connect(m_thread, SIGNAL(tilePainted(int,QPixmap*)), this, SLOT(onTilePainted(int,QPixmap*)), Qt::QueuedConnection);
  m_thread->start();
}
/*----------------------------------------------------------------------------*/
WaveFormWidget :: ~WaveFormWidget()
{
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: clear()
{
  int size = m_tiles.size();
  for(int i = 0; i < size; i++)
    if(m_tiles[i].pixmap != NULL)
      delete m_tiles[i].pixmap;
  m_tiles.clear();
  m_thread->clear();
  m_painted = false;
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: erase(int i)
{
  TileList::iterator it = m_tiles.begin() + i;
  if(it->pixmap != NULL)
    delete it->pixmap;
  m_tiles.erase(it);
  m_painted = false;
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: init(QSize size)
{
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
  clear();
  QSize tileSize(1 + size.width()/m_tileCount, size.height());
  //QSize tileSize(m_tileSize, m_wav->format().channels() * 200);
  for(qint64 i = 0; i < m_tileCount; i++)
  {
    Tile t;
    t.index = (int) i;
    t.startPosition = i * m_tileSize + m_startPosition;
    t.pixmap = new QPixmap(tileSize);
    t.painted = false;
    m_tiles.append(t);
  }
  m_painted = false;
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: drawTiles()
{
  int size = m_tiles.size();
  for(int i = 0; i < size; i++)
  {
    if(!m_tiles[i].painted)
    {
    //  drawTile(m_tiles[i]);
      m_thread->addTile(m_tiles[i]);
      m_tiles[i].pixmap = NULL;
    }
  }
  m_painted = true;
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: redrawTiles()
{
  int size = m_tiles.size();
  for(int i = 0; i < size; i++)
     drawTile(m_tiles[i]);
  m_painted = true;
}
/*----------------------------------------------------------------------------*/
void WaveFormWidget :: drawTile(Tile &tile)
{
  QPixmap *pixmap = tile.pixmap;
  if(pixmap == NULL)
    return;

  QPainter painter(pixmap);

  int channels = m_wav->format().channels();

  QPen pen(m_fgColor);
  painter.setPen(pen);

  painter.fillRect(pixmap->rect(), m_bgColor);

  qreal dX = (qreal)pixmap->width() / (qreal) m_tileSize;
  qreal dY = (qreal)pixmap->height() / (2 * (qreal) channels);

  QVector<QLine> lines(channels);
  QVector<int> baseY(channels);

  QPen pen1(m_gridColor);
  painter.setPen(pen1);
  for(int i = 0; i < channels; i++)
  {
    baseY[i] = (int)(dY + dY * i * 2);
    lines[i].setP1(QPoint(0, baseY[i]));
    lines[i].setP2(QPoint(pixmap->width(), baseY[i]));
    painter.drawLine(lines[i]);
  }


  //Draw signal
  painter.setPen(pen);
  if(tile.startPosition) //read last point
  {
    m_wav->seek(tile.startPosition - 1, *m_in);
    QVector<qreal> data = m_wav->readNext(*m_in);
    int c = data.size();
    for(int i = 0; i < channels && i < c; i++)
    {
      qreal y = data[i] * dY + baseY[i];
      lines[i].setP1(QPoint(0, (int)y));
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
      lines[i].setP2(QPoint((int)x, (int)y));
      painter.drawLine(lines[i]);
      lines[i].setP1(QPoint((int)x, (int)y));
    }
  }
  tile.painted = true;
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
  if(!m_windowSize || m_wav == NULL || m_tiles.empty())
    return;

  qreal dX = (qreal)width() / (qreal) m_windowSize;

//  painter.drawPixmap(0, 0, m_pixmap);
  int size = m_tiles.size();
  for(int i = 0; i < size; i++)
  {
    if(m_tiles[i].painted && m_tiles[i].pixmap != NULL)
      painter.drawPixmap((int)(i * m_tileSize * dX), 0, *m_tiles[i].pixmap);
  }

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
    painter1.fillRect(QRect(0,0,r.width(), r.height()), m_fgColor);
    for(int i = 0; i < size; i++)
    {
      if(m_tiles[i].pixmap == NULL)
        continue;
      int x = (int)(i * m_tileSize * dX);
      if(x < r.right() && x + m_tiles[i].pixmap->width() >= r.left())
        painter1.drawPixmap(QPoint(x - r.left(), 0), *m_tiles[i].pixmap);
    }

    img.invertPixels();
    painter.drawImage(r.left(), r.top(), img);
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

  bool modified = m_position != pos;
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
    if(!m_painted)
    {
      modified = true;
      drawTiles();
    }
    if(modified)
      update();
    return;
  }


  if(!m_painted || ABS(m_startPosition - newStart) >= m_windowSize)
  {
    int size = m_tiles.size();
    m_startPosition = newStart;
    for(int i = 0; i < size; i++)
    {
      m_tiles[i].index = i;
      m_tiles[i].startPosition = m_tileSize * i + m_startPosition;
      m_tiles[i].painted = false;
    }
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
    int size = m_tiles.size();
    for(int i = 0; i < size; i++)
    {
      m_tiles[i].index = i;
      m_tiles[i].startPosition = m_tileSize * i + m_startPosition;
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
void WaveFormWidget :: onTilePainted(int index, QPixmap *pixmap)
{
  if(index >= 0 && index <= m_tiles.size())
  {
    m_tiles[index].pixmap = pixmap;
    m_tiles[index].painted = true;
    update();
  }
}
/*----------------------------------------------------------------------------*/
