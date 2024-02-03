#include "Arduino.h"
#include "TimeManager.h"


TimeManager::TimeManager(const uint32_t millis_period)
	: _period(millis_period)
	, _last_millis(millis()) 
{
}

void TimeManager::SetPeriod(const uint32_t millis_period) {
	_period = millis_period;
	ResetTimer();
}

bool TimeManager::IsReady() {
	if (millis() - _last_millis >= _period) {
		ResetTimer();
		return true;
	}
	return false;
}

void TimeManager::ResetTimer() {
	_last_millis = millis();
}
