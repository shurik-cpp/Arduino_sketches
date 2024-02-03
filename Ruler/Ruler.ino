#include "TimeManager.h"
#include "RotaryRuler.h"
#include "LiquidCrystal_I2C.h"
#include <EEPROM.h>

const uint8_t ENC_YELLOW_WIRE = 2;
const uint8_t ENC_GREEN_WIRE = 3;
// 1024 импульса энкодера == на 0.5 оборота
constexpr uint16_t ENC_RESOLUTION = 1024 * 2;

RotaryRuler ruler(ENC_YELLOW_WIRE, ENC_GREEN_WIRE);

LiquidCrystal_I2C lcd(0x3F, 16, 2);
bool isLcdClear = true;

enum class MenuItem {
	MEASURE,
	SET_REVERSE,
	SET_DIAMETER,
	SET_RESOLUTION,
	SET_PRECISION,
	SAVE_IN_EEPROM
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
	bool isDistanseChanged = false;
	bool isInc = false;
	bool isDec = false;
} events;

void EventsTick();
void EventsReset();
void MeasureMode();
void SetReverseMode();
void SaveSettings();
void ClearLcdLineTo(const uint8_t line, const uint8_t index);

template <typename T>
uint8_t GetDigitsCount(const T number, const uint8_t precision = 0);

void setup() {
	// Serial.begin(9600);
	
	lcd.begin();
	lcd.setCursor(0, 0);
	lcd.print(F("Rotary Ruler"));

	uint8_t data = 0;
	EEPROM.get(0, data);
	const bool isSavedData = (data == 1);
	if (isSavedData) {
		EEPROM.get(deviceSettings.index, deviceSettings);
	}

	ruler.setDiameter(deviceSettings.diameter_mm);
	ruler.setReverse(deviceSettings.isReverse);
	ruler.setResolution(deviceSettings.enc_resolution);

	isLcdClear = true;
	currentScreen = MenuItem::MEASURE;
}

void loop() {
	EventsTick();
	switch (currentScreen) {
		case MenuItem::MEASURE:
			MeasureMode();
		break;
		case MenuItem::SET_REVERSE:
			SetReverseMode();
		break;
		case MenuItem::SET_DIAMETER:
		break;
		case MenuItem::SET_RESOLUTION:
		break;
		case MenuItem::SET_PRECISION:
		break;
		case MenuItem::SAVE_IN_EEPROM:
			SaveSettings();
		break;
		default:
		break;
	}
	EventsReset();
}

inline void EventsTick() {
	events.isDistanseChanged = ruler.isDistanseChanged();
	events.isInc = ruler.isIncremented();
	events.isDec = ruler.isDecremented();
}

inline void EventsReset() {
	events.isDistanseChanged = false;
	events.isInc = false;
	events.isDec = false;
}

void MeasureMode() {
	if (isLcdClear) {
		lcd.clear();
		isLcdClear = false;
	}

	static TimeManager dirTimer(static_cast<uint32_t>(Time::SEC_0_5));
	if (events.isInc || events.isDec) {
		dirTimer.ResetTimer();
		lcd.setCursor(0, 0);
		lcd.print(F("Rotary Ruler"));
		if (events.isInc) {
		lcd.setCursor(15, 0);
			lcd.print('>');
		}
		else if (events.isDec) {
		lcd.setCursor(15, 0);
			lcd.print('<');
		}
	}
	else if (dirTimer.IsReady()) {
		lcd.setCursor(15, 0);
		lcd.print(' ');
	}

	if (events.isDistanseChanged) {
		const float distance = ruler.getDistance();
		const int posInLine = 15 - GetDigitsCount<float>(distance, deviceSettings.precision) - 3;
		ClearLcdLineTo(1, posInLine);
		lcd.print(distance, deviceSettings.precision);
		lcd.print(F(" mm"));
	}
}

void SetReverseMode() {
	if (isLcdClear) {
		lcd.clear();
		isLcdClear = false;
		lcd.setCursor(0, 0);
		lcd.print(F("Reverse ruler:"));
		const uint8_t charCount = deviceSettings.isReverse ? 3 : 2;
		const int posInLine = 15 - charCount;
		lcd.setCursor(posInLine, 1);
		lcd.print(deviceSettings.isReverse ? "YES" : "NO");
	}
	
	if (events.isInc || events.isDec) {
		deviceSettings.isReverse = !deviceSettings.isReverse;
		ruler.setReverse(deviceSettings.isReverse);
		const uint8_t charCount = deviceSettings.isReverse ? 3 : 2;
		const int posInLine = 15 - charCount;
		ClearLcdLineTo(1, posInLine);
		lcd.print(deviceSettings.isReverse ? "YES" : "NO");
	}
}

void SaveSettings() {
	static bool newSaving = true;
	static TimeManager showMessageTimer(static_cast<uint32_t>(Time::SEC_1));
	if (newSaving) {
		const uint8_t isSave = 0;
		EEPROM.put(0, isSave);
		EEPROM.put(deviceSettings.index, deviceSettings);
		showMessageTimer.ResetTimer();
		newSaving = false;
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("Settings"));
		lcd.setCursor(0, 1);
		lcd.print(F("saved"));
	}
	if (showMessageTimer.IsReady()) {
		currentScreen = MenuItem::MEASURE;
		newSaving = true;
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