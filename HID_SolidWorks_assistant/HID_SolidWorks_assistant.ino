#include <Keyboard.h>
#include <EncButton2.h>
#include "joystick.h"
#include "time_manager.h"

const int SERIAL_SPEED = 9600;
const uint8_t PIN_VCC = 5;

const int REPEAT_PRESSING = 10;
const char CTRL_KEY = KEY_LEFT_CTRL;

enum InputStick {
	VRX = 18,
	VRY,
	SW
};

enum InputEnc {
	KEY = 3,
	S2,
	S1
};

EncButton2<EB_ENCBTN> encoder(INPUT_PULLUP, InputEnc::S1, InputEnc::S2, InputEnc::KEY);
Joystick joystick(InputStick::VRX, InputStick::VRY, InputStick::SW);

TimeManager timer(REPEAT_PRESSING);

void TestInput();


void setup() {
	// подаем питание на энкодер
	pinMode(PIN_VCC, OUTPUT);
	digitalWrite(PIN_VCC, HIGH);
	Serial.begin(SERIAL_SPEED);
	pinMode(InputEnc::KEY, INPUT_PULLUP);
	pinMode(InputEnc::S1, INPUT_PULLUP);
	pinMode(InputEnc::S2, INPUT_PULLUP);

}

void loop() {
	encoder.tick();
	joystick.Tick();

	//TestInput();

	if (timer.IsReady()) {
		if (digitalRead(InputStick::SW)) {
			if (joystick.IsPlusX()) {
				Keyboard.press(KEY_LEFT_ALT);
				Keyboard.press(KEY_RIGHT_ARROW);
			}
			if (joystick.IsMinusX()) {
				Keyboard.press(KEY_LEFT_ALT);
				Keyboard.press(KEY_LEFT_ARROW);
			}
			if (joystick.IsPlusY()) {
				Keyboard.press(KEY_LEFT_SHIFT);
				Keyboard.press(KEY_UP_ARROW);
			}
			if (joystick.IsMinusY()) {
				Keyboard.press(KEY_LEFT_SHIFT);
				Keyboard.press(KEY_DOWN_ARROW);
			}
		}
		else {
			Keyboard.press(CTRL_KEY);
			if (joystick.IsPlusX()) {
				Keyboard.press(KEY_RIGHT_ARROW);
			}
			if (joystick.IsMinusX()) {
				Keyboard.press(KEY_LEFT_ARROW);
			}
			if (joystick.IsPlusY()) {
				Keyboard.press(KEY_UP_ARROW);
			}
			if (joystick.IsMinusY()) {
				Keyboard.press(KEY_DOWN_ARROW);
			}
		}
	}

	if (digitalRead(InputEnc::KEY)) {
		if (encoder.right()) {
			Keyboard.press(KEY_LEFT_SHIFT);
			Keyboard.press(KEY_RIGHT_ARROW);
		}
		if (encoder.left()) {
			Keyboard.press(KEY_LEFT_SHIFT);
			Keyboard.press(KEY_LEFT_ARROW);
		}
	}
	else {
		if (encoder.right()) {
			Keyboard.press(CTRL_KEY);
			Keyboard.press('=');
		}
		if (encoder.left()) {
			Keyboard.press(CTRL_KEY);
			Keyboard.press('-');
		}
	}
	
	if (joystick.IsZero()) {
		//Keyboard.press(KEY_ESC);
		Keyboard.releaseAll();
	}
}

void TestInput() {
	float stick_x_val = analogRead(InputStick::VRX);
	float stick_y_val = analogRead(InputStick::VRY);

	if (joystick.IsPlusX_press()) {
		Serial.print("X+\t");
		Serial.println(stick_x_val);
	}
	if (joystick.IsMinusX_press()) {
		Serial.print("X-\t");
		Serial.println(stick_x_val);
	}
	if (joystick.IsPlusY_press()) {
		Serial.print("Y+\t");
		Serial.println(stick_y_val);
	}
	if (joystick.IsMinusY_press()) {
		Serial.print("Y-\t");
		Serial.println(stick_y_val);
	}
	if (joystick.IsButton_press()) {
		Serial.println("Joystick click!");
	}
	if (encoder.right()) {
		Serial.println("Encoder turn right");
	}
	if (encoder.left()) {
		Serial.println("Encoder turn left");
	}
	if (encoder.isPress()) {
		Serial.println("Encoder click!");
	}
}