#include "TimeManager.h"
#include "RotaryRuler.h"
#include "LiquidCrystal_I2C.h"

const uint8_t ENC_YELLOW_WIRE = 2;
const uint8_t ENC_GREEN_WIRE = 3;
// 1024 импульса энкодера == на 0.5 оборота
constexpr uint16_t ENC_RESOLUTION = 1024 * 2;

uint16_t diameter_mm = 11;

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
	ButtonTick();
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
	lcd.setCursor(15, 0);
	static TimeManager dirTimer(static_cast<uint32_t>(Time::SEC_0_5));
	if (events.isInc || events.isDec) {
		dirTimer.ResetTimer();
		if (events.isInc) {
			lcd.print('>');
		}
		else if (events.isDec) {
			lcd.print('<');
		}
	}
	else if (dirTimer.IsReady()) {
		lcd.print(' ');
	}

	if (events.isDistanseChanged) {
		lcd.setCursor(0, 0);
		lcd.print(F("Rotary Ruler"));

		lcd.setCursor(0, 1);
		lcd.print(ruler.getDistance(), 2);
		lcd.print(F(" mm             "));

		// Serial.print(ruler.getDistance(), 3);
		// Serial.print(" mm, inc: "); Serial.print(isInc);
		// Serial.print(", dec: "); Serial.println(isDec);
	}
}

void SetReverseMode() {
	// TODO: Вывод информации на экран
	if (events.isInc || events.isDec) {
		ruler.setReverse(!ruler.getReverseMode());
	}
}
