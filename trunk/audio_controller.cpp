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
#include "audio_controller.h"
#include <QAudioOutput>
#include <QtDebug>
/*----------------------------------------------------------------------------*/
AudioController :: AudioController(QObject *parent)
  :QThread(parent)
{
  m_wavFile = NULL;
  m_audioOutput = NULL;
}
/*----------------------------------------------------------------------------*/
AudioController :: ~AudioController()
{
  quit();
  wait(100);

  if(m_wavFile != NULL)
    delete m_wavFile;
  if(m_audioOutput != NULL)
    delete m_audioOutput;
}
/*----------------------------------------------------------------------------*/
bool AudioController :: checkOpened()
{
  if(m_wavFile == NULL)
  {
    emit error(tr("Undefile WAV file name"));
    return false;
  }
  return true;
}
/*----------------------------------------------------------------------------*/
bool AudioController :: open(const QString& fileName)
{
  QFile file(fileName);
  if(!file.open(QIODevice::ReadOnly))
  {
    emit error(tr("Error open file `%1`\n%2").arg(fileName, file.errorString()));
    return false;
  }

  if(m_wavFile != NULL)
    delete m_wavFile;
  m_wavFile = NULL;


  m_wavFile = new WavFile(fileName, this);
  if(!m_wavFile->readHeader(file))
  {
    emit error(tr("Error read wavefile header"));
    delete m_wavFile;
    m_wavFile = NULL;
    return false;

  }
  emit wavOpen(m_wavFile);
  return true;
}
/*----------------------------------------------------------------------------*/
bool AudioController :: open(QFile &file)
{
  if(!checkOpened())
    return false;

  file.setFileName(m_wavFile->fileName());
  if(!file.open(QIODevice::ReadOnly))
  {
    emit error(tr("Error open file `%1`:%2").arg(m_wavFile->fileName(), file.errorString()));
    return false;
  }
  return seek(0, file);
}
/*----------------------------------------------------------------------------*/
void AudioController :: close()
{
  if(m_wavFile != NULL)
    delete m_wavFile;
  m_wavFile = NULL;
  if(m_audioOutput != NULL)
    delete m_audioOutput;
  m_audioOutput = NULL;
  emit wavClose();
}
/*----------------------------------------------------------------------------*/
/**Return audio duration in us*/
qint64 AudioController :: duration()
{
  if(!checkOpened())
    return 0;
  return m_wavFile->duration();
}
/*----------------------------------------------------------------------------*/
/**Return sample count*/
qint64 AudioController :: length()
{
  if(!checkOpened())
    return 0;
  return m_wavFile->length();
}
/*----------------------------------------------------------------------------*/
/**Return true if controller in play state*/
bool AudioController :: isPlay()
{
  if(m_wavFile == NULL || m_audioOutput == NULL)
    return false;
  if(m_audioOutput->state() == QAudio::ActiveState)
    return true;
  return false;
}
/*----------------------------------------------------------------------------*/
const QAudioFormat& AudioController :: format() const
{
  if(m_wavFile != NULL)
    return m_wavFile->format();
  return m_format;
}
/*----------------------------------------------------------------------------*/
/**Conver duration tims in us to number of quants*/
qint64 AudioController :: duration2quants(qint64 duration)
{
  return format().frequency() * duration / 1000000;
}
/*----------------------------------------------------------------------------*/
/**Conver number of quants to duration tims in us*/
qint64 AudioController :: quants2duration(qint64 quants)
{
  return (quants * 1000000) / format().frequency();
}
/*----------------------------------------------------------------------------*/
/**Reads next quant value from stream*/
QVector<qreal> AudioController :: readNext(QIODevice &device)
{
  if(!checkOpened())
    return QVector<qreal>();
  return m_wavFile->readNext(device);
}
/*----------------------------------------------------------------------------*/
/**Seek to quants stream position*/
bool AudioController :: seek(qint64 quants, QIODevice &device)
{
  if(!checkOpened())
    return false;
  if(m_wavFile->seek(quants, device))
    return true;
  emit error(tr("Error seek device: %1").arg(device.errorString()));
  return false;
}
/*----------------------------------------------------------------------------*/
void AudioController :: run()
{
  if(m_audioOutput != NULL)
    delete m_audioOutput;

  qDebug() << "Start AudioController thread";
  m_audioOutput = new QAudioOutput(format());
  QFile *file = new QFile(m_audioOutput);
  if(!open(*file) || !seek(m_playStart, *file))
    return;
  m_audioOutput->setNotifyInterval(10);
  connect(m_audioOutput, SIGNAL(notify()), this, SLOT(onOutputNotify()), Qt::QueuedConnection);
  connect(m_audioOutput, SIGNAL(stateChanged(QAudio::State)), this, SLOT(onOtputStateChanged(QAudio::State)));
  m_audioOutput->start(file);
  onOutputNotify();
  exec();
  qDebug() << "End AudioController thread";
}
/*----------------------------------------------------------------------------*/
bool AudioController :: play(qint64 quantsFrom, qint64 quantsLen)
{
  if(!checkOpened())
    return false;
  if(isPlay())
  {
    emit error(tr("Audio already in use"));
    return false;
  }

  if(quantsLen == -1)
    quantsLen = length();


  m_playDuration = quants2duration(quantsLen);
  m_playStart = quantsFrom;

  //QThread::start(QThread::TimeCriticalPriority);
  QThread::start(QThread::HighPriority);
  return true;
}
/*----------------------------------------------------------------------------*/
void AudioController :: stop()
{
  if(m_audioOutput != NULL)
    m_audioOutput->stop();
  if(isRunning())
    quit();
}
/*----------------------------------------------------------------------------*/
void AudioController :: onOtputStateChanged(QAudio::State state)
{
  if (QAudio::StoppedState == state)
  {
    QAudio::Error err = m_audioOutput->error();
    if(QAudio::NoError != err)
    {
      switch(err)
      {
      case QAudio::OpenError:
        emit error(tr("An error opening the audio device"));
        break;
      case QAudio::IOError:
        emit error(tr("An error occurred during read/write of audio device"));
        break;
      case QAudio::UnderrunError:
        emit error(tr("Audio data is not being fed to the audio device at a fast enough rate"));
        break;
      case QAudio::FatalError:
        emit error(tr("A non-recoverable error has occurred, The audio device is not usable at this time."));
        break;
      default:
        emit error(tr("Undefined error has occurred"));
        break;
      }
    }
  }

  if(QAudio::ActiveState == state)
    emit startPlay();
  else
  {
    if(isRunning())
      quit();
    emit stopPlay();
  }
}
/*----------------------------------------------------------------------------*/
void AudioController :: onOutputNotify()
{
  if(m_audioOutput == NULL)
    return;

  qint64 usPlayed = m_audioOutput->processedUSecs();
  emit playPosition(m_playStart + duration2quants(usPlayed));
  if(usPlayed >= m_playDuration)
    m_audioOutput->stop();

}
/*----------------------------------------------------------------------------*/
