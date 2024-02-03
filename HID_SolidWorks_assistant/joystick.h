#pragma once
#include <EncButton2.h>

class Joystick {
public:
	Joystick(const uint8_t pin_x, const uint8_t pin_y, const uint8_t pin_button);
	Joystick() = delete;
	~Joystick() = default;

	void Tick();
	bool IsPlusX();
	bool IsMinusX();
	bool IsPlusY();
	bool IsMinusY();
	bool IsZero();
	bool IsPlusX_press();
	bool IsMinusX_press();
	bool IsPlusY_press();
	bool IsMinusY_press();
	bool IsButton_press();
	bool IsButtonClickWithPlusX();
	bool IsButtonClickWithMinusX();
	bool IsButtonClickWithPlusY();
	bool IsButtonClickWithMinusY();

private:
	EncButton2<EB_BTN> button;
	const uint8_t _pin_x;
	const uint8_t _pin_y;
	bool x_once = true;
	bool y_once = true;
	const int JOYSTIC_PLUS = 1000;
	const int JOYSTIC_MINUS = 10;

};