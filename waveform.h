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
#include <QPixmap>
#include <QVector>

class WavFile;
class QFile;
/*----------------------------------------------------------------------------*/
class WaveFormWidget : public QWidget
{
  Q_OBJECT

private:
  qint64 m_startPosition;
  qint64 m_windowDurationUs;
  qint64 m_timeRuler;
  qint64 m_windowSize;
  qint64 m_position;

  qint64 m_selectionStart;
  qint64 m_selectionLength;

  WavFile *m_wav;
  QFile *m_in;
  QPixmap m_pixmap;

public:
  WaveFormWidget(QWidget *parent);
  virtual ~WaveFormWidget();

  void setWavFile(WavFile *w);
  qint64 windowDuration() const {return m_windowDurationUs;}
  qint64 windowLength() const {return m_windowSize;}
  qint64 selectionStart() const {return m_selectionStart;}
  qint64 selectionLength() const {return m_selectionLength;}


protected:
  virtual void paintEvent(QPaintEvent *event);
  virtual void resizeEvent(QResizeEvent *event);
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent * event);
  virtual void mouseMoveEvent (QMouseEvent * event);

  void drawPixmap(const QSize& size);
  qint64 pixel2audio(int x) const;
  void setStartSelection(int x);
  void setEndSelection(int x);

public slots:
  void setFilePosition(qint64 position);
  void wavFileOpened(WavFile *w);
  void wavFileClosed();
};

/*----------------------------------------------------------------------------*/
#endif /*WAVEFORM_H_1291720077*/

