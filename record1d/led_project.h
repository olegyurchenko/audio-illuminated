/*----------------------------------------------------------------------------*/
/**
* @pkg led_project
*/
/**
* Short description of led_project.
*
* Long description of led_project.<br>
* (C) T&T, Kiev, Ukraine 2010.<br>
* started 30.11.2010  9:19:39<br>
* @pkgdoc led_project
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef LED_PROJECT_H_1291101579
#define LED_PROJECT_H_1291101579
#include <QString>
#include <QList>
#include <QTime>
#include <QVector>
#include <QObject>
#include <QDataStream>
/*----------------------------------------------------------------------------*/
#include <led.h>

typedef struct
{
  int time;
  QVector<int> leds;
} LedRecord;

QDataStream& operator <<(QDataStream& out, const LedRecord& r);
QDataStream& operator >>(QDataStream& in, LedRecord& r);
/*----------------------------------------------------------------------------*/
class QSound;
class LedProject : public QObject
{
  Q_OBJECT

  typedef QList<LedRecord> LedRecordList;
protected:
  bool pmodified;
  QTime time;
  QString pfileName;
  QString pwavFile;
  LedRecordList recordList;
  QSound *sound;
public:
  LedProject(QObject *parent = NULL);
  virtual ~LedProject();

  void start();
  void stop();
  void addRecord(LedVector *leds); //Record
  void getRecord(LedVector *leds); //Play

  bool readFromFile(const QString& fileName);
  bool writeToFile(const QString& fileName = QString());

  bool modified() {return pmodified;}
  QString wavFile() {return pwavFile;}
  QString fileName() {return pfileName;}
  void setWavFile(const QString& file) {pmodified = pmodified || pwavFile != file; pwavFile = file;}
  void clear() {pmodified = false; recordList.clear(); pwavFile = pfileName = QString(); }
  void clearRecords() {pmodified = pmodified || !recordList.isEmpty(); recordList.clear(); }
  static bool isSoundAvailable();
signals:
  void stoped();
};
/*----------------------------------------------------------------------------*/
#endif /*LED_PROJECT_H_1291101579*/

