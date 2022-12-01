#include "DallasTemperature.h"
#include "time_manager.h"

enum PinsDefine {
	ONE_WIRE_PIN = 7,
	RELAY_PIN = 8,
	LED = 13
};

const float temp_heating_on = 15.0;
const float temp_heating_off = 20.0;
const uint32_t relay_delay_millis = 1'800'000; // 30 минут
const int SERIAL_SPEED = 9600;

enum SensorResolution {
    TEMP_9_BIT = 9,
    TEMP_10_BIT,
    TEMP_11_BIT,
    TEMP_12_BIT
};
OneWire one_wire(ONE_WIRE_PIN);
DallasTemperature sensor(&one_wire);

TimeManager sensor_research_timer(Time::SEC_1);
TimeManager sensor_tick(Time::SEC_1);
TimeManager relay_timer(relay_delay_millis);


void SensorSearch();

void setup() {
	Serial.begin(SERIAL_SPEED);
	pinMode(PinsDefine::RELAY_PIN, OUTPUT);
	pinMode(PinsDefine::LED, OUTPUT);
	
}

void loop() {
	static float temperature = DEVICE_DISCONNECTED_C;
	const bool is_sensor_enable = static_cast<int>(temperature) == DEVICE_DISCONNECTED_C;
	
	if (!is_sensor_enable && sensor_research_timer.IsReady()) {
		SensorSearch();
		sensor_research_timer.ResetTimer();
	}
	if (is_sensor_enable && sensor_tick.IsReady()) {
		temperature = sensor.getTempCByIndex(0);
	}
	
	
	
	

}

void SensorSearch() {
	Serial.println("Search sensor...")
	sensor.begin();
	if (sensor.getDS18Count()) {
		sensor.setResolution(SensorResolution::TEMP_12_BIT);
		sensor.requestTemperatures();
		sensor_tick.ResetTimer();
	}
	else {
		sensor_research_timer.ResetTimer();
	}
}