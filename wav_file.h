/*----------------------------------------------------------------------------*/
/**
* @pkg wav_file
*/
/**
* Class to work with WAV file.
*
* Class to load WAV, can read parts and get samples.<br>
* (C) T&T, Kiev, Ukraine 2010.<br>
* started 01.12.2010 10:03:10<br>
* @pkgdoc wav_file
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef WAV_FILE_H_1291190590
#define WAV_FILE_H_1291190590
/*----------------------------------------------------------------------------*/
#include <QString>
#include <QObject>
#include <QFile>
#include <QAudioFormat>
#include <QVector>

class WavFile : public QObject
{
Q_OBJECT
protected:
  QString m_fileName;
  QAudioFormat m_format;
  qint64 m_dataLength;

public:
  WavFile(const QString& name = QString(), QObject *parent = NULL);
  virtual ~WavFile();

  void setFileName(const QString& name) {m_fileName = name;}
  QString fileName() const {return m_fileName;}

  const QAudioFormat& format() const {return m_format;}

  qint64 duration() const;
  qint64 length(qint64 microSeconds = -1) const;

  // Reads WAV header and seeks to start of data
  bool readHeader(QIODevice &device);
  // Reads next quant value from stream
  QVector<qreal> readNext(QIODevice &device) const;

  bool seek(qint64 points, QIODevice &device) const;
  qint64 pos(QIODevice &device);

protected:
  qint64 audioDuration(qint64 bytes) const;
  qint64 audioLength(qint64 microSeconds) const;

  static qint64 headerLength();
  qint64 dataLength() const {return m_dataLength;}
};
/*----------------------------------------------------------------------------*/
#endif /*WAV_FILE_H_1291190590*/

