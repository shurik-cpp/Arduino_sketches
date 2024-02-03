#line 1 "D:\\home_shurik\\Arduino\\Ruler\\RotaryRuler.h"
#pragma once
#include <stdint.h>

class RotaryRuler {
private:
	int32_t _lastEncoderTickCounter = 0;
	uint16_t _resolution = 0;
	float _diameter = 0;
	int _incDecGisteresis = 0;
	int32_t _lastIncDecTickCounter = 0;
	bool _isInc = false;
	bool _isDec = false;

public:
	RotaryRuler() = delete;
	explicit RotaryRuler(const uint8_t yellowWire, const uint8_t greenWire, const uint16_t _resolution);
	~RotaryRuler() = default;

	void setReverse(const bool value);
	bool getReverseMode() const;
	void setResolution(const uint16_t resolution) { _resolution = resolution; }
	uint16_t getResolution() const { return _resolution; }
	void setDiameter(const float diameter) { _diameter = diameter; }
	float getDiameter() { return _diameter; }

	void resetMeasurement();
	bool isDistanseChanged();
	int32_t getEncoderTickCounter() const;
	float getTuroverCounter() const;
	float getDistance() const;
	bool isIncremented();
	bool isDecremented();

private:
	void setInterrupts(const bool enable);
	void calcDirection();
};