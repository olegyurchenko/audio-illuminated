/*----------------------------------------------------------------------------*/
/**
* @pkg led_effects
*/
/**
* Short description of led_effects.
*
* Long description of led_effects.<br>
* (C) T&T, Kiev, Ukraine 2010.<br>
* started 29.11.2010 13:08:45<br>
* @pkgdoc led_effects
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef LED_EFFECTS_H_1291028925
#define LED_EFFECTS_H_1291028925
/*----------------------------------------------------------------------------*/
#include <led.h>
#include <QStringList>
class LedEffects
{
public:
  LedEffects();
  virtual ~LedEffects();

  bool execute(int effectIndex, LedVector *leds, int selectedLed = -1);
  QStringList getEffects();
};
/*----------------------------------------------------------------------------*/
#endif /*LED_EFFECTS_H_1291028925*/

