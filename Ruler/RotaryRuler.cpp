#include "RotaryRuler.h"
#include <Arduino.h>

namespace RullerInterrupt {

	uint8_t _yellowWire = 0;
	uint8_t _greenWire = 0;
	bool _isReverse = false;
	volatile int32_t encoderTickCounter = 0;

	void Interrupt_0() {
		if (!digitalRead(_greenWire))
			encoderTickCounter = _isReverse ? encoderTickCounter - 1 : encoderTickCounter + 1;
		else
			encoderTickCounter = _isReverse ? encoderTickCounter + 1 : encoderTickCounter - 1;
	}

	void Interrupt_1() {
		if (!digitalRead(_yellowWire))
			encoderTickCounter = _isReverse ? encoderTickCounter + 1 : encoderTickCounter - 1;
		else
			encoderTickCounter = _isReverse ? encoderTickCounter - 1 : encoderTickCounter + 1;
	}
} // namespace Ruller 

RotaryRuler::RotaryRuler(const uint8_t yellowWire, const uint8_t greenWire, const uint16_t resolution) 
	: _resolution(resolution)
	, _incDecGisteresis(resolution / 16)
	{
		RullerInterrupt::_yellowWire = yellowWire;
		RullerInterrupt::_greenWire = greenWire;
    }

void RotaryRuler::init() {
	// setPinsMode
	pinMode(RullerInterrupt::_yellowWire, INPUT);
	pinMode(RullerInterrupt::_greenWire, INPUT);
	// turn on pullup resistors
	digitalWrite(RullerInterrupt::_yellowWire, HIGH);
	digitalWrite(RullerInterrupt::_greenWire, HIGH);
	// setInterrupts
	attachInterrupt(0, RullerInterrupt::Interrupt_0, RISING);
	attachInterrupt(1, RullerInterrupt::Interrupt_1, RISING);
}

int32_t RotaryRuler::getEncoderTickCounter() const {
	return RullerInterrupt::encoderTickCounter;
}

float RotaryRuler::getTuroverCounter() const {
	return (float)RullerInterrupt::encoderTickCounter / _resolution;
}

float RotaryRuler::getDistance() const {
    return _diameter * PI * getTuroverCounter();
}

bool RotaryRuler::isIncremented() {
	const bool result = _isInc && !_isDec;
	_isInc = false;
	return result;
}

bool RotaryRuler::isDecremented() {
	const bool result = !_isInc && _isDec;
	_isDec = false;
	return result;
}

void RotaryRuler::resetMeasurement() {
	_lastEncoderTickCounter = 0;
    RullerInterrupt::encoderTickCounter = 0;
}

bool RotaryRuler::isDistanseChanged() {
    if (_lastEncoderTickCounter != RullerInterrupt::encoderTickCounter) {
		if (abs(_lastIncDecTickCounter - RullerInterrupt::encoderTickCounter) >= _incDecGisteresis) {
			if (_lastEncoderTickCounter < RullerInterrupt::encoderTickCounter) {
				_isInc = true;
				_isDec = false;
				// Serial.println("_lastEncoderTickCounter < RullerInterrupt::encoderTickCounter");
			}
			else if (_lastEncoderTickCounter > RullerInterrupt::encoderTickCounter) {
				_isInc = false;
				_isDec = true;
				// Serial.println("_lastEncoderTickCounter > RullerInterrupt::encoderTickCounter");
			}
			_lastIncDecTickCounter = RullerInterrupt::encoderTickCounter;
		}
		_lastEncoderTickCounter = RullerInterrupt::encoderTickCounter;
		return true;
	}
	return false;
}

void RotaryRuler::setReverse(const bool value) {
	RullerInterrupt::_isReverse = value;
}
