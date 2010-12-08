/*----------------------------------------------------------------------------*/
/**
* @pkg led
*/
/**
* Short description of led.
*
* Long description of led.<br>
* (C) T&T, Kiev, Ukraine 2010.<br>
* started 29.11.2010 11:25:46<br>
* @pkgdoc led
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef LED_H_1291022746
#define LED_H_1291022746
#include <QTime>
#include <QVector>
/*----------------------------------------------------------------------------*/
class Led
{
  int pvalue;
  QTime start;
public:
  Led(){pvalue = 1; setValue(0);}
  virtual ~Led() {}
  int value() const {return pvalue;}
  void setValue(int v);
  bool isOn() const {return pvalue != 0;}
  bool isOff() const {return pvalue == 0;}
  void setOn() {setValue(1);}
  void setOff() {setValue(0);}
  int elapsed() const {return start.elapsed();}
};
/*----------------------------------------------------------------------------*/
typedef QVector<Led> LedVector;
#endif /*LED_H_1291022746*/

