#include "ilTimer.h"

ilTimer ilTimerControl;

#define ilTimer_mode_free			B00000000
#define ilTimer_mode_timeout		B00000001
#define ilTimer_mode_interval 		B00000010
#define ilTimer_mode_manual		B00000100

#define ilTimer_status_stop		B10000000
#define ilTimer_status_running 	B01000000
#define ilTimer_status_pause		B00100000

#define tMODE(t)					this->timer[t].mode
#define tMS(t)						this->timer[t].ms
#define tREPEATSTOTAL(t)			this->timer[t].repeatsTotal
#define tREPEATS(t)					this->timer[t].repeats
#define tTIMEOUT(t)					this->timer[t].timeout
#define tCALLBACK(t)				this->timer[t].callback

#define _IS_FREE(t)					(tMODE(t) & 0x0F)==ilTimer_mode_free
#define _IS_TIMEOUT(t)				(tMODE(t) & 0x0F)==ilTimer_mode_timeout
#define _IS_INTERVAL(t)				(tMODE(t) & 0x0F)==ilTimer_mode_interval
#define _IS_MANUAL(t)				(tMODE(t) & 0x0F)==ilTimer_mode_manual

#define _SET_FREE(t)				tMODE(t)=(tMODE(t) & 0xF0) | ilTimer_mode_free
#define _SET_TIMEOUT(t)				tMODE(t)=(tMODE(t) & 0xF0) | ilTimer_mode_timeout
#define _SET_INTERVAL(t)			tMODE(t)=(tMODE(t) & 0xF0) | ilTimer_mode_interval
#define _SET_MANUAL(t)				tMODE(t)=(tMODE(t) & 0xF0) | ilTimer_mode_manual

#define _IS_STOP(t)					(tMODE(t) & 0xF0)==ilTimer_status_stop
#define _IS_RUNNING(t)				(tMODE(t) & 0xF0)==ilTimer_status_running
#define _IS_PAUSE(t)				(tMODE(t) & 0xF0)==ilTimer_status_pause

#define _SET_STOP(t)				tMODE(t)=(tMODE(t) & 0x0F) | ilTimer_status_stop
#define _SET_RUNNING(t)				tMODE(t)=(tMODE(t) & 0x0F) | ilTimer_status_running
#define _SET_PAUSE(t)				tMODE(t)=(tMODE(t) & 0x0F) | ilTimer_status_pause

#define _CHECK_Timer_				if (t>ilTimer_max_timers || _IS_FREE(t)) return -1;
#define _LOOP_						for (int t=0;t<ilTimer_max_timers;t++)

#define _SET 						tMS(t)=ms;\
									tREPEATSTOTAL(t)=repeatsBeforeCallback;\
									tREPEATS(t)=tREPEATSTOTAL(t);\
									tCALLBACK(t)=callback;

#define _PRINT						Serial.println("----"); _LOOP_ Serial.println(_IS_FREE(t));

ilTimer::ilTimer(){
	_LOOP_
		_SET_FREE(t);

	this->count=0;
}

void ilTimer::check(){
	byte checks=0;
	byte currentCount=this->count;

	_LOOP_{
		if (!_IS_FREE(t)){
			checks++;

			if (_IS_RUNNING(t) && millis()>tTIMEOUT(t)){

				while (millis()>tTIMEOUT(t))
						tTIMEOUT(t)=tTIMEOUT(t) + tMS(t);

				if (tREPEATSTOTAL(t)>0){
					tREPEATS(t)--;
					if (tREPEATS(t)>0)
						continue;
				}

				if (_IS_INTERVAL(t)){
					tREPEATS(t)=tREPEATSTOTAL(t);
				}

				if (_IS_TIMEOUT(t)){
					this->free(t);
				}

				if (_IS_MANUAL(t)){
					tREPEATS(t)=tREPEATSTOTAL(t);
					_SET_STOP(t);
				}

				(*tCALLBACK(t))((byte)t);
			}
		}

		if (checks==currentCount)
			break;
	}
}

int ilTimer::getFree(){
	_LOOP_
		if (_IS_FREE(t))
			return t;

	return -1;
}

int ilTimer::timeout(void(*callback)(byte),unsigned int ms,unsigned int repeatsBeforeCallback){
	int t=this->getFree();
	if (t<0)
		return -1;

	_SET_TIMEOUT(t);
	_SET;
	this->_reset(t);

	this->count++;

	return t;
}

int ilTimer::interval(void(*callback)(byte),unsigned int ms,unsigned int repeatsBeforeCallback){
	int t=this->getFree();
	if (t<0)
		return -1;

	_SET_INTERVAL(t);
	_SET;
	this->_reset(t);

	this->count++;

	return t;
}

int ilTimer::manual(void(*callback)(byte),unsigned int ms,unsigned int repeatsBeforeCallback){
	int t=this->getFree();
	if (t<0)
		return -1;

	_SET_MANUAL(t);
	_SET;

	this->count++;

	return t;
}

int ilTimer::stop(byte t){
	_CHECK_Timer_

	_SET_STOP(t);

	return t;
}

int ilTimer::_reset(byte t){
	_SET_RUNNING(t);
	tREPEATS(t)=tREPEATSTOTAL(t);
	tTIMEOUT(t)=millis()+tMS(t);
}

int ilTimer::reset(byte t,unsigned int new_ms,unsigned int repeatsBeforeCallback){
	_CHECK_Timer_

	if (new_ms!=0){
		tMS(t)=new_ms;
		tREPEATSTOTAL(t)=repeatsBeforeCallback;
	}

	this->_reset(t);

	return t;
}

int ilTimer::free(byte t){
	_CHECK_Timer_

	_SET_FREE(t);
	this->count--;

	return t;
}
