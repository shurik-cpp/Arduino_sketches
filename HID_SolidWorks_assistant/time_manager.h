#pragma once

//#include <cstdint>

enum Time {
    SEC_1 = 1000,
    SEC_2 = 2000,
    SEC_3 = 3000,
    SEC_4 = 4000,
    SEC_5 = 5000,
};

class TimeManager {
public:
	TimeManager() = default;
	TimeManager(const uint32_t millis_period);
	~TimeManager() = default;

	// проверять состояние таймера каждый тик. вернет true, когда таймер готов. 
	bool IsReady(); 

	void SetPeriod(const uint32_t millis_period);

private:
	uint32_t _last_millis;
	uint32_t _period = 0;
};