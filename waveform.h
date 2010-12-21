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
#ifndef WAVEFORM_H_1291720077
#define WAVEFORM_H_1291720077
/*----------------------------------------------------------------------------*/
#include <QWidget>
#include <QImage>
#include <QVector>
#include <QColor>
#include <QList>
#include <QPoint>
#include <QThread>
#include <QMap>
#include <QMutex>
#include <QSemaphore>

class WavFile;
class QFile;
class WaveFormThread;
/*----------------------------------------------------------------------------*/
class WaveFormWidget : public QWidget
{
  Q_OBJECT

protected:
  qint64 m_startPosition;
  qint64 m_windowDurationUs;
  qint64 m_timeRuler;
  qint64 m_windowSize;
  qint64 m_position;

  qint64 m_selectionStart;
  qint64 m_selectionLength;

  QColor m_bgColor;
  QColor m_fgColor;
  QColor m_gridColor;
  QColor m_markerColor;


  WavFile *m_wav;
  QFile *m_in;

  qint64 m_tileDuration;
  qint64 m_tileSize;
  qint64 m_tileCount;
  int m_noPainted;

  typedef struct
  {
    int index;
    qint64 startPosition;
    bool painted;
    QImage *image;
  } Tile;

  typedef QList<Tile> TileList;
  TileList m_tiles;
  WaveFormThread *m_thread;
public:
  WaveFormWidget(QWidget *parent);
  virtual ~WaveFormWidget();

  void setWavFile(WavFile *w);
  qint64 windowDuration() const {return m_windowDurationUs;}
  qint64 windowLength() const {return m_windowSize;}
  qint64 selectionStart() const {return m_selectionStart;}
  qint64 selectionLength() const {return m_selectionLength;}

  QColor bgColor(){return m_bgColor;}
  QColor fgColor(){return m_fgColor;}
  QColor gridColor(){return m_gridColor;}
  QColor markerColor(){return m_markerColor;}

protected:
  virtual void paintEvent(QPaintEvent *event);
  virtual void resizeEvent(QResizeEvent *event);
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent * event);
  virtual void mouseMoveEvent (QMouseEvent * event);

  void init(QSize size);
  void clear();
  void erase(int i);
  void drawTiles();
  void redrawTiles();
  void drawTile(Tile &tile);

  qint64 pixel2audio(int x) const;
  void setStartSelection(int x);
  void setEndSelection(int x);

public slots:
  void setFilePosition(qint64 position);
  void wavFileOpened(WavFile *w);
  void wavFileClosed();
  void setBgColor(QColor c);
  void setFgColor(QColor c);
  void setGridColor(QColor c);
  void setMarkerColor(QColor c);
  void setWindowDuration(qint64 duration);
protected slots:
  void onTilePainted(int index, int transNo);
  void onTilesReady();
signals:
  void windowStartChanged(qint64);

  friend class WaveFormThread;
};
/*----------------------------------------------------------------------------*/
class WaveFormThread : public QThread
{
  Q_OBJECT
protected:
  typedef QMap<int, WaveFormWidget::Tile> TileMap;
  TileMap m_tiles;
  bool m_terminated;
  QSemaphore m_sem;
  QMutex m_mut;
  QMutex m_global;
  int m_transNo;
  WaveFormWidget *m_waveform;
public:
  WaveFormThread(WaveFormWidget *parent);
  virtual ~WaveFormThread();
  virtual void run();
  void addTile(const WaveFormWidget::Tile& tile);
  void clear();
  void clearQueue();
  int transNo();

  void lock() {m_global.lock();}
  void unlock(){m_global.unlock();}

signals:
  void tilePainted(int index, int transNo);
  void ready();
};
/*----------------------------------------------------------------------------*/
#endif /*WAVEFORM_H_1291720077*/

