#ifndef _Battery_h
#define _Battery_h

#include <Arduino.h>

#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>

#define SLEEP_15MS WDTO_15MS
#define SLEEP_30MS WDTO_30MS
#define SLEEP_60MS WDTO_60MS
#define SLEEP_120MS WDTO_120MS
#define SLEEP_250MS WDTO_250MS
#define SLEEP_500MS WDTO_500MS
#define SLEEP_1S WDTO_1S
#define SLEEP_2S WDTO_2S
#define SLEEP_4S WDTO_4S
#define SLEEP_8S WDTO_8S

uint32_t batteryRead()
{
  uint32_t result;
  ADMUX = 0b00100001;
  delay(2);
  ADCSRA |= _BV(ADSC);
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result;
  return result;
}

static void powerOff(const uint8_t sleep_time=SLEEP_8S)
{
  wdt_enable(sleep_time);
  WDTCSR |= _BV(WDIE);
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  
  clock_prescale_set(clock_div_256);
  power_all_disable();
  
  sleep_mode();
}

static void powerOn()
{
  if (F_CPU == 1000000UL)
    clock_prescale_set(clock_div_8);
  else
    clock_prescale_set(clock_div_1);
  
  wdt_disable();
  
  sleep_disable();
  
  power_all_enable();
}

void sleep(uint32_t sleepTime)
{
  uint8_t offTime;

  while (sleepTime > 15)
  {
    if (sleepTime >= 8000) { offTime = SLEEP_8S; sleepTime -= 8000; }
    else if (sleepTime >= 4000) { offTime = SLEEP_4S; sleepTime -= 4000; }
    else if (sleepTime >= 2000) { offTime = SLEEP_2S; sleepTime -= 2000; }
    else if (sleepTime >= 1000) { offTime = SLEEP_1S; sleepTime -= 1000; }
    else if (sleepTime >= 500) { offTime = SLEEP_500MS; sleepTime -= 500; }
    else if (sleepTime >= 250) { offTime = SLEEP_250MS; sleepTime -= 250; }
    else if (sleepTime >= 120) { offTime = SLEEP_120MS; sleepTime -= 120; }
    else if (sleepTime >= 60) { offTime = SLEEP_60MS; sleepTime -= 60; }
    else if (sleepTime >= 30) { offTime = SLEEP_30MS; sleepTime -= 30; }
    else if (sleepTime >= 15) { offTime = SLEEP_15MS; sleepTime -= 15; }

    powerOff(offTime);
  }
  powerOn();
}

ISR (WDT_vect)
{
}

#endif
