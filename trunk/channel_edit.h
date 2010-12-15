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
#ifndef CHANNEL_EDIT_H_1291969403
#define CHANNEL_EDIT_H_1291969403
/*----------------------------------------------------------------------------*/
#include <QWidget>
#include <wav_file.h>
#include <QColor>
#include <effect_controller.h>
#include <QPolygon>

class ChannelEdit: public QWidget
{
  Q_OBJECT
private:
  typedef QWidget Inherited;

  qint64 m_startPosition;
  qint64 m_windowDurationUs;
  qint64 m_timeRuler;
  qint64 m_windowSize;
  qint64 m_position;

  QColor m_bgColor;
  QColor m_fgColor;
  QColor m_gridColor;
  QColor m_markerColor;
  int m_channelId;
  int m_selected;

  bool m_audioOpened;

  EffectController::PropPointList effects;

public:
  ChannelEdit(QWidget *parent);
  virtual ~ChannelEdit();

  qint64 windowDuration() const {return m_windowDurationUs;}
  qint64 windowLength() const {return m_windowSize;}

  QColor bgColor(){return m_bgColor;}
  QColor fgColor(){return m_fgColor;}
  QColor gridColor(){return m_gridColor;}
  QColor markerColor(){return m_markerColor;}

  int channelId() {return m_channelId;}

protected:
  virtual void paintEvent(QPaintEvent *event);
  virtual void resizeEvent(QResizeEvent *event);
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent * event);
  virtual void mouseMoveEvent (QMouseEvent * event);

  void updateData();
  QPolygon createPolygon(int x);

public slots:
  void setFilePosition(qint64 position);
  void wavFileOpened(WavFile *w);
  void wavFileClosed();
  void setBgColor(QColor c);
  void setFgColor(QColor c);
  void setGridColor(QColor c);
  void setMarkerColor(QColor c);
  void setWindowDuration(qint64 duration);
  void setWindowStart(qint64 quants);
  void setChannelId(int i);
  void onChangeEditMode();
  void onSelectionChanged();

};

/*----------------------------------------------------------------------------*/
#endif /*CHANNEL_EDIT_H_1291969403*/

