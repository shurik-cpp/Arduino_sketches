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
	RotaryRuler() = default;
	explicit RotaryRuler(const uint8_t yellowWire, const uint8_t greenWire, const uint16_t _resolution);
	~RotaryRuler() = default;

	void init();
	bool isDistanseChanged();
	void resetMeasurement();
	void setReverse(const bool value);
	void setResolution(const uint16_t resolution) { _resolution = resolution; }
	void setDiameter(const float diameter) { _diameter = diameter; }
	int32_t getEncoderTickCounter() const;
	float getTuroverCounter() const;
	float getDistance() const;
	bool isIncremented();
	bool isDecremented();
	
};