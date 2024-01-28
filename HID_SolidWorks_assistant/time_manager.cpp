#include "Arduino.h"
#include "time_manager.h"


TimeManager::TimeManager(const uint32_t millis_period)
	: _period(millis_period)
	, _last_millis(millis()) 
{
}

void TimeManager::SetPeriod(const uint32_t millis_period) {
	_period = millis_period;
	_last_millis = millis();
}

bool TimeManager::IsReady() {
	if (millis() - _last_millis >= _period) {
		_last_millis = millis();
		return true;
	}
	return false;
}