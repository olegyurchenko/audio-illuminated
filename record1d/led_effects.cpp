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
#include "led_effects.h"
/*----------------------------------------------------------------------------*/
//TODO: Read plugins with effects
LedEffects :: LedEffects()
{
}
/*----------------------------------------------------------------------------*/
LedEffects :: ~LedEffects()
{
}
/*----------------------------------------------------------------------------*/
QStringList LedEffects :: getEffects()
{
  QStringList l;
  l.append("Simple switch");
  l.append("OneShot leds");
  l.append("*0*0*0");
  l.append("*00*00*00");
  l.append("*000*000*000");
  l.append("*0000*0000*0000");
  l.append("******000000000");
  l.append("000000000******");
  return l;
}
/*----------------------------------------------------------------------------*/
bool LedEffects :: execute(int effectIndex, LedVector *leds, int selectedLed)
{
  if(effectIndex == 0) //Simple switch on selected led
  {
    LedVector::iterator end = leds->end();
    int i = 0;
    for(LedVector::iterator it = leds->begin(); it != end; it++)
    {
      if(selectedLed == i)
        it->setOn();
      else
        it->setOff();
      i++;
    }
  }
  else
  if(effectIndex == 1) //OneShot for all leds
  {
    LedVector::iterator end = leds->end();
    int i = 0;
    for(LedVector::iterator it = leds->begin(); it != end; it++)
    {
      if(selectedLed == i)
        it->setOn();
      else
      if(it->isOn() && it->elapsed() > 100)
      {
        it->setOff();
      }
      i++;
    }
  }
  else
  if(effectIndex == 2) // *0*0*0
  {
    LedVector::iterator end = leds->end();
    int i = 0;
    int phase = 2;
    if(selectedLed >= 0)
      phase = selectedLed % 2;
    for(LedVector::iterator it = leds->begin(); it != end; it++)
    {
      if(phase == i)
      {
        it->setOn();
        phase += 2;
      }
      else
        it->setOff();
      i++;
    }
  }
  else
    if(effectIndex == 3) // *00*00*00
    {
      LedVector::iterator end = leds->end();
      int i = 0;
      int phase = 3;
      if(selectedLed >= 0)
        phase = selectedLed % 3;
      for(LedVector::iterator it = leds->begin(); it != end; it++)
      {
        if(phase == i)
        {
          it->setOn();
          phase += 3;
        }
        else
          it->setOff();
        i++;
      }
    }
    else
  if(effectIndex == 4) // *000*000*000
  {
    LedVector::iterator end = leds->end();
    int i = 0;
    int phase = 4;
    if(selectedLed >= 0)
      phase = selectedLed % 4;
    for(LedVector::iterator it = leds->begin(); it != end; it++)
    {
      if(phase == i)
      {
        it->setOn();
        phase += 4;
      }
      else
        it->setOff();
      i++;
    }
  }
  else
  if(effectIndex == 5) // *0000*0000*0000
  {
    LedVector::iterator end = leds->end();
    int i = 0;
    int phase = 5;
    if(selectedLed >= 0)
      phase = selectedLed % 5;
    for(LedVector::iterator it = leds->begin(); it != end; it++)
    {
      if(phase == i)
      {
        it->setOn();
        phase += 5;
      }
      else
        it->setOff();
      i++;
    }
  }
  else
  if(effectIndex == 6) //******000000000
  {
    LedVector::iterator end = leds->end();
    int i = 0;
    for(LedVector::iterator it = leds->begin(); it != end; it++)
    {
      if(selectedLed >= i)
        it->setOn();
      else
        it->setOff();
      i++;
    }
  }
  else
  if(effectIndex == 7) //000000000******
  {
    LedVector::iterator end = leds->end();
    int i = 0;
    for(LedVector::iterator it = leds->begin(); it != end; it++)
    {
      if(selectedLed < i)
        it->setOn();
      else
        it->setOff();
      i++;
    }
  }
  return true;
}
/*----------------------------------------------------------------------------*/

