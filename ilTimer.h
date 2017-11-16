/**
  Copyright MIT (@2015) ilausuch@gmail.com

  IMPORTANT: To implement more than 60 seconds, use repeatsBeforeCallback. For instance, one minute
  60000 ms + 60 repeats before callback
*/


#ifndef __IL_TIMER_H__
#define __IL_TIMER_H__

#include <Arduino.h>

#ifndef ilTimer_max_timers
  #define ilTimer_max_timers	10
#endif


typedef struct ilTimerItem{
	byte mode;
	unsigned int ms;
  unsigned int repeatsTotal;
  unsigned int repeats;
  unsigned long timeout;
  void(*callback)(byte);
};

class ilTimer{
  public:
    ilTimer();
    void check();
    int timeout(void(*callback)(byte),unsigned int ms,unsigned int repeatsBeforeCallback=0);
    int interval(void(*callback)(byte),unsigned int ms,unsigned int repeatsBeforeCallback=0);
    int manual(void(*callback)(byte),unsigned int ms,unsigned int repeatsBeforeCallback=0);
    int stop(byte timer);
    int reset(byte timer,unsigned int new_ms=0,unsigned int repeatsBeforeCallback=0);
    int free(byte timer);


  private:
  	int getFree();
    int _reset(byte t);

  	ilTimerItem		timer[ilTimer_max_timers];
  	byte 				count;
};

extern ilTimer ilTimerControl;

#endif
