/*----------------------------------------------------------------------------*/
/**
* @pkg audio_controller
*/
/**
* Class to controll audio output.
*
* Class to read data from WAV file, and playback audio.<br>
* Copyright (C) 2010 Oleg Yurchenko, Kiev, Ukraine.<br>
* started 08.12.2010 11:03:26<br>
* @pkgdoc audio_controller
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef AUDIO_CONTROLLER_H_1291799006
#define AUDIO_CONTROLLER_H_1291799006
#include <QThread>
#include <wav_file.h>
#include <QFile>
#include <QVector>
#include <QAudio>
/*----------------------------------------------------------------------------*/
class QAudioOutput;
class AudioController : public QThread
{
  Q_OBJECT
  Q_PROPERTY(WavFile* wavFile READ wavFile)
  Q_PROPERTY(qint64 duration READ duration)
  Q_PROPERTY(qint64 length READ length)
  Q_PROPERTY(bool isPlay READ isPlay)
  Q_PROPERTY(const QAudioFormat& format READ format)

private:
  WavFile *m_wavFile;
  QAudioOutput *m_audioOutput;
  qint64 m_playDuration;
  qint64 m_playStart;

  QAudioFormat m_format;
public:
  AudioController(QObject *parent = NULL);
  virtual ~AudioController();

  bool open(const QString& fileName);
  bool open(QFile &file);
  void close();

  WavFile *wavFile() {return m_wavFile;}

  /**Return audio duration in us*/
  qint64 duration();
  /**Return sample count*/
  qint64 length();
  /**Return true if controller in play state*/
  bool isPlay();

  const QAudioFormat& format() const;

  /**Conver duration tims in us to number of quants*/
  qint64 duration2quants(qint64 duration);
  /**Conver number of quants to duration tims in us*/
  qint64 quants2duration(qint64 quants);

  /**Reads next quant value from stream*/
  QVector<qreal> readNext(QIODevice &device);
  /**Seek to quants stream position*/
  bool seek(qint64 quants, QIODevice &device);

protected:
  void run();

public slots:
  bool play(qint64 quantsFrom = 0, qint64 quantsLen = -1);
  void stop();

protected slots:
  void onOtputStateChanged(QAudio::State state);
  void onOutputNotify();
private:
  bool checkOpened();

signals:
  void wavOpen(WavFile *);
  void wavClose();
  void playPosition(qint64 samples);
  void error(const QString& message);
  void startPlay();
  void stopPlay();
};

extern AudioController* audioController;
/*----------------------------------------------------------------------------*/
#endif /*AUDIO_CONTROLLER_H_1291799006*/

