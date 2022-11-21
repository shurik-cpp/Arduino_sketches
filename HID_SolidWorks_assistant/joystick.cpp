#include "joystick.h"

Joystick::Joystick(const uint8_t pin_x, const uint8_t pin_y, const uint8_t pin_button) 
	: button(EncButton2<EB_BTN>(INPUT_PULLUP, pin_button))
	, _pin_x(pin_x)
	, _pin_y(pin_y)
{
}

void Joystick::Tick() {
	button.tick();
	if (!x_once && !IsPlusX() && !IsMinusX()) {
		x_once = true;
	}
	if (!y_once && !IsPlusY() && !IsMinusY()) {
		y_once = true;
	}
}

bool Joystick::IsPlusX_press() {
	if (x_once && IsPlusX()) {
		x_once = false;
		return true;
	}
	return false;
}
bool Joystick::IsMinusX_press() {
	if (x_once && IsMinusX()) {
		x_once = false;
		return true;
	}
	return false;
}
bool Joystick::IsPlusY_press() {
	if (y_once && IsPlusY()) {
		y_once = false;
		return true;
	}
	return false;
}
bool Joystick::IsMinusY_press() {
	if (y_once && IsMinusY()) {
		y_once = false;
		return true;
	}
	return false;
}

bool Joystick::IsButtonClickWithPlusX() {
	return IsPlusX() && button.isClick();
}
bool Joystick::IsButtonClickWithMinusX() {
	return IsMinusX() && button.isClick();
}
bool Joystick::IsButtonClickWithPlusY() {
	return IsPlusY() && button.isClick();
}
bool Joystick::IsButtonClickWithMinusY() {
	return IsMinusY() && button.isClick();
}


bool Joystick::IsZero() {
	return !this->IsPlusX() && !this->IsMinusX() && !this->IsPlusY() && !this->IsMinusY(); 
}

bool Joystick::IsPlusX() {
	return analogRead(_pin_x) > JOYSTIC_PLUS;
}
bool Joystick::IsMinusX() {
	return analogRead(_pin_x) < JOYSTIC_MINUS;
}

bool Joystick::IsPlusY() {
	return analogRead(_pin_y) < JOYSTIC_MINUS;
}
bool Joystick::IsMinusY() {
	return analogRead(_pin_y) > JOYSTIC_PLUS;
}
bool Joystick::IsButton_press() {
	return button.isPress();
}
