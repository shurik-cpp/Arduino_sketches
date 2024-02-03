#include "TimeManager.h"
#include "RotaryRuler.h"
#include "LiquidCrystal_I2C.h"

const uint8_t ENC_YELLOW_WIRE = 2;
const uint8_t ENC_GREEN_WIRE = 3;
// 1024 импульса энкодера == на 0.5 оборота
constexpr uint16_t ENC_RESOLUTION = 1024 * 2;

uint16_t diameter_mm = 11;
uint8_t precisionForFloat = 2;

RotaryRuler ruler(ENC_YELLOW_WIRE, ENC_GREEN_WIRE, ENC_RESOLUTION);

LiquidCrystal_I2C lcd(0x3F, 16, 2);

enum class MenuItem {
	MEASURE,
	SET_REVERSE,
	SET_DIAMETER,
	SET_RESOLUTION,
	SAVE_IN_EEPROM
} currentScreen;

struct {
	bool isDistanseChanged = false;
	bool isInc = false;
	bool isDec = false;
} events;

void EventsTick();
void EventsReset();
void ButtonTick();
void MeasureMode();
void SetReverseMode();

template <typename T>
uint8_t GetDigitsCount(const T number, const uint8_t precision = 0);

void setup() {
	// Serial.begin(9600);
	
	lcd.begin();
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print(F("Rotary Ruler"));

	ruler.setDiameter(diameter_mm);
	ruler.setReverse(true);

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
		default:
		break;
	}
	EventsReset();
}

inline void EventsTick() {
	ButtonTick();
	events.isDistanseChanged = ruler.isDistanseChanged();
	events.isInc = ruler.isIncremented();
	events.isDec = ruler.isDecremented();
}

inline void EventsReset() {
	events.isDistanseChanged = false;
	events.isInc = false;
	events.isDec = false;
}

void ButtonTick()
{
    // TODO: Обработать нажатие кнопки
}

void MeasureMode() {
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
		const int posInLine = 15 - GetDigitsCount<float>(distance, precisionForFloat) - 3;
		lcd.setCursor(0, 1);
		for (int i = 0; i <= posInLine; ++i) {
			lcd.print(' '); // очистим пробелами перед выводом информации
		}
		lcd.print(distance, precisionForFloat);
		lcd.print(F(" mm"));
	}
}

void SetReverseMode() {
	// TODO: Вывод информации на экран
	if (events.isInc || events.isDec) {
		ruler.setReverse(!ruler.getReverseMode());
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