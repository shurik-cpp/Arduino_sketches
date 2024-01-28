#include "RotaryRuler.h"

const uint8_t ENC_YELLOW_WIRE = 2;
const uint8_t ENC_GREEN_WIRE = 3;
// 1024 импульса энкодера == на 0.5 оборота
constexpr uint16_t ENC_RESOLUTION = 1024 * 2;

uint16_t diameter_mm = 11;

RotaryRuler ruler(ENC_YELLOW_WIRE, ENC_GREEN_WIRE, ENC_RESOLUTION);

void setup() {
	Serial.begin(9600);
	ruler.setDiameter(diameter_mm);
	ruler.setReverse(true);
}

void loop() {
	// static TimeManager serialTimer(static_cast<uint32_t>(Time::SEC_1));

	if (ruler.isDistanseChanged()) {
		Serial.print(ruler.getDistance(), 3);
		// Serial.println(" mm"); 
		Serial.print(" mm, inc: "); Serial.print(ruler.isIncremented());
		Serial.print(", dec: "); Serial.println(ruler.isDecremented());
	}
}
