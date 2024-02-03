#include "TimeManager.h"
#include "RotaryRuler.h"
#include "LiquidCrystal_I2C.h"
#include <EEPROM.h>
#include <EncButton.h>

const uint8_t ENC_YELLOW_WIRE = 2;
const uint8_t ENC_GREEN_WIRE = 3;
// 1024 импульса энкодера == на 0.5 оборота
constexpr uint16_t ENC_RESOLUTION = 1024 * 2;

RotaryRuler ruler(ENC_YELLOW_WIRE, ENC_GREEN_WIRE);

LiquidCrystal_I2C lcd(0x3F, 16, 2);

const uint8_t BUTTON_PIN = 7;
const uint8_t BUTTON_GND = 8;

enum class MenuItem {
	MEASURE,
	SET_REVERSE,
	SET_DIAMETER,
	SET_RESOLUTION,
	SET_PRECISION,
	SAVE_IN_EEPROM,
	SAVED
} currentScreen;

struct Settings {
	uint8_t index = 1;
	bool isReverse = true;
	uint16_t diameter_mm = 20;
	uint16_t enc_resolution = ENC_RESOLUTION;
	uint8_t precision = 2;
};

Settings deviceSettings;
struct {
	bool isNewScreen = false;
	bool isDistanseChanged = false;
	bool isInc = false;
	bool isDec = false;
	bool isButtonClick = false;
	bool isButtonHold = false;
} events;

void debugEvents();
void EventsTick();
void EventsHandle();
void EventsReset();
void DrawMeasure();
void DrawSetReverse();
void DrawSetDiameter();
void DrawSetResolution();
void DrawSetPrecision();
void SaveSettings();
MenuItem GetNextScreen();
void ClearLcdLineTo(const uint8_t line, const uint8_t index);

template <typename T>
uint8_t GetDigitsCount(const T number, const uint8_t precision = 0);

void setup() {
	Serial.begin(9600);
	pinMode(BUTTON_GND, OUTPUT);
	digitalWrite(BUTTON_GND, LOW);

	lcd.begin();
	lcd.setCursor(0, 0);
	lcd.print(F("Rotary Ruler"));

	uint8_t data = 255;
	EEPROM.get(0, data);
	Serial.print("EEPROM.get(0, data) == ");
	Serial.println(data);
	const bool isSavedData = (data == 1);
	if (isSavedData) {
		EEPROM.get(deviceSettings.index, deviceSettings);
		Serial.println("Get deviceSettings from EEPROM");
	}

	ruler.setDiameter(deviceSettings.diameter_mm);
	ruler.setReverse(deviceSettings.isReverse);
	ruler.setResolution(deviceSettings.enc_resolution);

	events.isNewScreen = true;
	currentScreen = MenuItem::MEASURE;
}

void loop() {
	EventsTick();
	debugEvents();
	switch (currentScreen) {
		case MenuItem::MEASURE:
			DrawMeasure();
		break;
		case MenuItem::SET_REVERSE:
			DrawSetReverse();
		break;
		case MenuItem::SET_DIAMETER:
			DrawSetDiameter();
		break;
		case MenuItem::SET_RESOLUTION:
			DrawSetResolution();
		break;
		case MenuItem::SET_PRECISION:
			DrawSetPrecision();
		break;
		case MenuItem::SAVE_IN_EEPROM:
			SaveSettings();
		break;
		default:
		break;
	}
	EventsReset();
}

inline void debugEvents() {
	if (events.isButtonClick) {
		Serial.println("Event: buton click!");
		Serial.print("currentScreen == ");
		Serial.println(static_cast<int>(currentScreen));
	}
	if (events.isButtonHold) {
		Serial.println("Event: buton hold!");
		Serial.print("currentScreen == ");
		Serial.println(static_cast<int>(currentScreen));
	}
	if (events.isInc) {
		Serial.println("Event: ruler Inc!");
		Serial.print("currentScreen == ");
		Serial.println(static_cast<int>(currentScreen));
	}
	if (events.isDec) {
		Serial.println("Event: ruler Dec!");
		Serial.print("currentScreen == ");
		Serial.println(static_cast<int>(currentScreen));
	}
	if (events.isNewScreen) {
		Serial.println("Event: newScreen!");
		Serial.print("currentScreen == ");
		Serial.println(static_cast<int>(currentScreen));
	}
}

inline void EventsTick()
{
    static Button button(BUTTON_PIN);
	button.setHoldTimeout(Time::SEC_1);
	button.tick();

	events.isButtonClick = button.click();
	events.isButtonHold = button.hold();
	events.isDistanseChanged = ruler.isDistanseChanged();
	events.isInc = ruler.isIncremented();
	events.isDec = ruler.isDecremented();
	EventsHandle();
}

inline void EventsHandle() {
	switch (currentScreen) {
		case MenuItem::MEASURE:
			if (events.isButtonHold)
				currentScreen = GetNextScreen();
			if (events.isButtonClick) {
				Serial.print(ruler.getEncoderTickCounter());
				Serial.print("-->");
				ruler.resetMeasurement();
				Serial.println(ruler.getEncoderTickCounter());
			}
		break;
		case MenuItem::SET_REVERSE:
			if (events.isInc || events.isDec) {
				deviceSettings.isReverse = !deviceSettings.isReverse;
				ruler.setReverse(deviceSettings.isReverse);
			}
			if (events.isButtonHold)
				currentScreen = MenuItem::SAVE_IN_EEPROM;
			else if (events.isButtonClick) {
				currentScreen = GetNextScreen();
			}
		break;
		case MenuItem::SET_DIAMETER:
			if (events.isInc) 
				deviceSettings.diameter_mm = deviceSettings.diameter_mm >= 500 ? 500 : deviceSettings.diameter_mm + 1;
			else if (events.isDec)
				deviceSettings.diameter_mm = deviceSettings.diameter_mm == 0 ? 0 : deviceSettings.diameter_mm - 1;
			ruler.setDiameter(deviceSettings.diameter_mm);
			if (events.isButtonHold)
				currentScreen = MenuItem::SAVE_IN_EEPROM;
			else if (events.isButtonClick) {
				currentScreen = GetNextScreen();
			}
		break;
		case MenuItem::SET_RESOLUTION:
			if (events.isInc)
				deviceSettings.enc_resolution++;
			else if (events.isDec)
				deviceSettings.enc_resolution--;
			ruler.setResolution(deviceSettings.enc_resolution);
			if (events.isButtonHold)
				currentScreen = MenuItem::SAVE_IN_EEPROM;
			else if (events.isButtonClick) {
				currentScreen = GetNextScreen();
			}
		break;
		case MenuItem::SET_PRECISION:
			if (events.isInc)
				deviceSettings.precision = deviceSettings.precision >= 5 ? 5 : deviceSettings.precision + 1;
			else if (events.isDec)
				deviceSettings.precision = deviceSettings.precision == 0 ? 0 : deviceSettings.precision - 1;
				if (events.isButtonHold)
				currentScreen = MenuItem::SAVE_IN_EEPROM;
			else if (events.isButtonClick) {
				currentScreen = GetNextScreen();
			}
		break;
		default:
			currentScreen = GetNextScreen();
		break;
	}
}

inline MenuItem GetNextScreen() {
	events.isNewScreen = true;
	if (currentScreen == MenuItem::SAVED)
		return MenuItem::MEASURE;
	if (currentScreen == MenuItem::SET_PRECISION)
		return MenuItem::SET_REVERSE;
	
	const uint8_t screen = static_cast<uint8_t>(currentScreen) + 1;
	return static_cast<MenuItem>(screen);
}

inline void EventsReset() {
	events.isNewScreen = false;
	events.isDistanseChanged = false;
	events.isInc = false;
	events.isDec = false;
	events.isButtonClick = false;
	events.isButtonHold = false;
}

void DrawMeasure() {
	if (events.isNewScreen) {
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("Rotary Ruler"));
	}
	const float distance = ruler.getDistance();
	static float lastDistance = 0;
	static TimeManager dirTimer(static_cast<uint32_t>(Time::SEC_0_5));
	if (lastDistance != distance) {
		dirTimer.ResetTimer();
		if (lastDistance < distance) {
		lcd.setCursor(15, 0);
			lcd.print('>');
		}
		else if (lastDistance > distance) {
		lcd.setCursor(15, 0);
			lcd.print('<');
		}
	}
	else if (dirTimer.IsReady()) {
		lcd.setCursor(15, 0);
		lcd.print(' ');
	}

	if (events.isDistanseChanged || events.isNewScreen) {
		lastDistance = distance;
		uint8_t posInLine = 0;
		if (deviceSettings.precision > 0) {
			posInLine = 15 - GetDigitsCount<float>(distance, deviceSettings.precision) - 3;
			ClearLcdLineTo(1, posInLine);
			lcd.print(distance, deviceSettings.precision);
		}
		else {
			posInLine = 15 - GetDigitsCount<int>(static_cast<int>(distance), deviceSettings.precision) - 3;
			ClearLcdLineTo(1, posInLine);
			lcd.print(static_cast<int>(distance));
		}
		lcd.print(F(" mm"));
	}
}

void DrawSetReverse() {
	auto PrintValue = []() {
		const uint8_t charsCount = deviceSettings.isReverse ? 3 : 2;
		const int posInLine = 15 - charsCount;
		ClearLcdLineTo(1, posInLine);
		lcd.print(deviceSettings.isReverse ? "YES" : "NO");
	};

	if (events.isNewScreen) {
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("Reverse ruler:"));
		PrintValue();
	}
	
	if (events.isInc || events.isDec) {
		PrintValue();
	}
}

void DrawSetDiameter() {
	auto PrintValue = []() {
		const int posInLine = 15 - GetDigitsCount<uint16_t>(deviceSettings.diameter_mm) - 3;
		ClearLcdLineTo(1, posInLine);
		lcd.print(deviceSettings.diameter_mm);
		lcd.print(F(" mm"));
	};

	if (events.isNewScreen) {
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("Set diameter:"));
		PrintValue();
	}

	if (events.isInc || events.isDec) {
		PrintValue();
	}
}

inline void DrawSetResolution() {
	auto PrintValue = []() {
		const int posInLine = 15 - GetDigitsCount<uint16_t>(deviceSettings.enc_resolution) - 6;
		ClearLcdLineTo(1, posInLine);
		lcd.print(deviceSettings.enc_resolution);
		lcd.print(F(" ticks"));
	};

	if (events.isNewScreen) {
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("Set resolution:"));
		PrintValue();
	}

	if (events.isInc || events.isDec) {
		PrintValue();
	}
}

inline void DrawSetPrecision() {
	auto PrintValue = []() {
		int posInLine = 15 - deviceSettings.precision - 1;
		if (deviceSettings.precision > 0)
			posInLine--;
		ClearLcdLineTo(1, posInLine);
		lcd.print('0');
		if (deviceSettings.precision > 0)
			lcd.print('.');
		for (uint8_t i = 0; i < deviceSettings.precision; ++i)
			lcd.print('0');
	};

	if (events.isNewScreen) {
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("Set precision:"));
		PrintValue();
	}

	if (events.isInc || events.isDec) {
		PrintValue();
	}
}

void SaveSettings() {
	const uint8_t isSave = 1;
	EEPROM.put(0, isSave);
	EEPROM.put(deviceSettings.index, deviceSettings);
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print(F("Settings"));
	lcd.setCursor(15 - 5, 1);
	lcd.print(F("saved"));

	TimeManager showMessageTimer(static_cast<uint32_t>(Time::SEC_1));
	while (!showMessageTimer.IsReady()) {
		currentScreen = MenuItem::SAVED;
		ruler.resetMeasurement();
	}
}

inline void ClearLcdLineTo(const uint8_t line, const uint8_t index) {
	lcd.setCursor(0, line);
	for (int i = 0; i <= index; ++i) {
		lcd.print(' ');
	}
}

template <typename T>
inline uint8_t GetDigitsCount(const T number, const uint8_t precision = 0) {
	int count = 1; // 0 мы всегда выводим
	if (precision > 0)
		count += 1 + precision; // "0. + precision" мы всегда выводим
	if (number < 0) // посчитаем символ '-'
		count++;
	int inumber = static_cast<int>(number); // берем целую часть
	if (abs(inumber) == 0) 
		return count;

	while (abs(inumber) > 0) {
		inumber /= 10;
		count = abs(inumber) > 0 ? count + 1 : count;
	}
	return count;
}