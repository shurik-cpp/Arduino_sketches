#pragma once

enum Time {
    SEC_0_1 = 100,
    SEC_0_2 = 200,
    SEC_0_3 = 300,
    SEC_0_4 = 400,
    SEC_0_5 = 500,
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
	void ResetTimer();

private:
	uint32_t _last_millis;
	uint32_t _period = 0;
};

