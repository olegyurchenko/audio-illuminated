/*----------------------------------------------------------------------------*/
/**
* @pkg wav_file
*/
/**
* Class to work with WAV file.
*
* Class to load WAV, can write parts and get samples.<br>
* (C) T&T, Kiev, Ukraine 2010.<br>
* started 01.12.2010 10:03:10<br>
* @pkgdoc wav_file
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "wav_file.h"
#include <QtEndian>
#include <QAudioOutput>
/*----------------------------------------------------------------------------*/
struct chunk
{
    char        id[4];
    quint32     size;
};
/*----------------------------------------------------------------------------*/
struct RIFFHeader
{
    chunk       descriptor;     // "RIFF"
    char        type[4];        // "WAVE"
};
/*----------------------------------------------------------------------------*/
struct WAVEHeader
{
    chunk       descriptor;
    quint16     audioFormat;
    quint16     numChannels;
    quint32     sampleRate;
    quint32     byteRate;
    quint16     blockAlign;
    quint16     bitsPerSample;
};
/*----------------------------------------------------------------------------*/
struct DATAHeader
{
    chunk       descriptor;
};
/*----------------------------------------------------------------------------*/
struct CombinedHeader
{
    RIFFHeader  riff;
    WAVEHeader  wave;
    DATAHeader  data;
};
/*----------------------------------------------------------------------------*/
static const int HeaderLength = sizeof(CombinedHeader);
/*----------------------------------------------------------------------------*/
qint64 audioDuration(const QAudioFormat &format, qint64 bytes)
{
  return (bytes * 1000000) /
      (format.frequency() * format.channels() * (format.sampleSize() / 8));
}
/*----------------------------------------------------------------------------*/
qint64 audioLength(const QAudioFormat &format, qint64 microSeconds)
{
  return (format.frequency() * format.channels() * (format.sampleSize() / 8))
      * microSeconds / 1000000;
}
/*----------------------------------------------------------------------------*/
const qint16  PCMS16MaxValue     =  32767;
const quint16 PCMS16MaxAmplitude =  32768; // because minimum is -32768
/*----------------------------------------------------------------------------*/
qreal pcmToReal(qint16 pcm)
{
  return qreal(pcm) / PCMS16MaxAmplitude;
}
/*----------------------------------------------------------------------------*/
qint16 realToPcm(qreal real)
{
  return (qint16)(real * PCMS16MaxValue);
}
/*----------------------------------------------------------------------------*/
qint64 WavFile :: headerLength()
{
  return ::HeaderLength;
}
/*----------------------------------------------------------------------------*/
WavFile :: WavFile(const QString& name, QObject *parent)
  : QObject(parent)
{
  m_fileName = name;
  m_dataLength = 0;
}
/*----------------------------------------------------------------------------*/
WavFile :: ~WavFile()
{
}
/*----------------------------------------------------------------------------*/
bool WavFile :: readHeader(QIODevice &device)
{
  bool result = true;

  if (!device.isSequential())
      result = device.seek(0);
  // else, assume that current position is the start of the header

  if (result)
  {
    CombinedHeader header;
    result = (device.read(reinterpret_cast<char *>(&header), HeaderLength) == HeaderLength);
    if (result)
    {
      if ((memcmp(&header.riff.descriptor.id, "RIFF", 4) == 0
           || memcmp(&header.riff.descriptor.id, "RIFX", 4) == 0)
        && memcmp(&header.riff.type, "WAVE", 4) == 0
            && memcmp(&header.wave.descriptor.id, "fmt ", 4) == 0
            && header.wave.audioFormat == 1 // PCM
            )
        {
        if (memcmp(&header.riff.descriptor.id, "RIFF", 4) == 0)
          m_format.setByteOrder(QAudioFormat::LittleEndian);
        else
          m_format.setByteOrder(QAudioFormat::BigEndian);

        m_format.setChannels(qFromLittleEndian<quint16>(header.wave.numChannels));
        m_format.setCodec("audio/pcm");
        m_format.setFrequency(qFromLittleEndian<quint32>(header.wave.sampleRate));
        m_format.setSampleSize(qFromLittleEndian<quint16>(header.wave.bitsPerSample));

        switch(header.wave.bitsPerSample)
        {
        case 8:
          m_format.setSampleType(QAudioFormat::UnSignedInt);
          break;
        case 16:
          m_format.setSampleType(QAudioFormat::SignedInt);
          break;
        default:
          result = false;
        }

        m_dataLength = device.size() - HeaderLength;
      }
      else
      {
        result = false;
      }
    }
  }

  return result;
}
/*----------------------------------------------------------------------------*/
qint64 WavFile :: audioDuration(qint64 bytes) const
{
  return ::audioDuration(m_format, bytes);
}
/*----------------------------------------------------------------------------*/
qint64 WavFile :: audioLength(qint64 microSeconds) const
{
  return ::audioLength(m_format, microSeconds);
}
/*----------------------------------------------------------------------------*/
qint64 WavFile :: duration() const
{
  return audioDuration(m_dataLength);
}
/*----------------------------------------------------------------------------*/
qint64 WavFile :: length(qint64 microSeconds) const
{
  if(microSeconds == -1)
    return m_dataLength / (m_format.channels() * (m_format.sampleSize() / 8));
  return audioLength(microSeconds) / (m_format.channels() * (m_format.sampleSize() / 8));
}
/*----------------------------------------------------------------------------*/
QVector<qreal> WavFile :: readNext(QIODevice &device) const
{
  QVector<qreal> output;
  QVector<char> inputSample(2 * m_format.channels());
#if 1
  if (device.read(inputSample.data(), inputSample.count()))
  {
    for (int i = 0; i < m_format.channels(); i++)
    {
      const qint16* input = reinterpret_cast<const qint16*>(inputSample.data() + 2 * i);
      output.append(pcmToReal(qFromLittleEndian<qint16>(*input)));
    }
  }
#endif
  return output;
}
/*----------------------------------------------------------------------------*/
bool WavFile :: seek(qint64 points, QIODevice &device) const
{
#if 1
  qint64 bytes = headerLength() + points * m_format.channels() * (m_format.sampleSize() / 8);
  if (!device.isSequential())
    return device.seek(bytes);
  return false;
#else
  return true;
#endif
}
/*----------------------------------------------------------------------------*/
qint64 WavFile :: pos(QIODevice &device)
{
  return device.pos() / (m_format.channels() * (m_format.sampleSize() / 8));
}
/*----------------------------------------------------------------------------*/
