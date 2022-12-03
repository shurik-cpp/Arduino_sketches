#include "DallasTemperature.h"
#include "TimeManager.h"
#include "Trigger.h"

enum PinsDefine {
	ONE_WIRE_PIN = 7,
	RELAY_PIN = 8,
	LED = 13
};

const float temp_heating_on = 15.0;
const float temp_heating_off = 20.0;
const uint32_t relay_delay_millis = 1800000; // 30 минут
const int SERIAL_SPEED = 9600;

enum SensorResolution {
    TEMP_9_BIT = 9,
    TEMP_10_BIT,
    TEMP_11_BIT,
    TEMP_12_BIT
};
OneWire one_wire(ONE_WIRE_PIN);
DallasTemperature sensor(&one_wire);
float temperature = DEVICE_DISCONNECTED_C;

TimeManager sensor_research_timer(Time::SEC_1);
TimeManager sensor_tick_timer(Time::SEC_1);
TimeManager relay_timer(relay_delay_millis);


void SensorSearch();
void PrintSensorAddress();
void PrintRelayStatus(const bool status);
void PrintTemperature();
void FirstReadTemperature();

void setup() {
	Serial.begin(SERIAL_SPEED);
	pinMode(PinsDefine::RELAY_PIN, OUTPUT);
	pinMode(PinsDefine::LED, OUTPUT);
	digitalWrite(PinsDefine::RELAY_PIN, LOW);

	SensorSearch();	
	if (sensor.getDS18Count()) {
		digitalWrite(PinsDefine::LED, HIGH);
		delay(Time::SEC_0_2);
		digitalWrite(PinsDefine::LED, LOW);
	}
	else {
		uint8_t i = 0;
		while (i < 3) {
			digitalWrite(PinsDefine::LED, HIGH);
			delay(Time::SEC_0_1);
			digitalWrite(PinsDefine::LED, LOW);
			delay(Time::SEC_0_1);
			i++;
		}
		sensor_research_timer.ResetTimer();
	}
}

void loop() {
	static bool relay_status = false;
	const bool is_sensor_enable = static_cast<int>(temperature) != DEVICE_DISCONNECTED_C;
	
	if (!is_sensor_enable && sensor_research_timer.IsReady()) {
		SensorSearch();
	}
	if (is_sensor_enable && sensor_tick_timer.IsReady()) {
		temperature = sensor.getTempCByIndex(0);
		sensor.requestTemperatures();
	}
	
	if (is_sensor_enable) {
		const bool on_by_temp = !relay_status && temperature < temp_heating_on;
		static Trigger on_by_temp_trig;
		
		if (on_by_temp_trig.IsRisingFront(on_by_temp)) {
			relay_status = true;
			PrintRelayStatus(relay_status);
			PrintTemperature();
			relay_timer.ResetTimer();
		}
	
		const bool off_by_temp = temperature > temp_heating_off;
		if (off_by_temp) {
			relay_status = false;
			relay_timer.ResetTimer();
		}
		static Trigger off_by_temp_trig;
		if (off_by_temp_trig.IsRisingFront(off_by_temp)) {
			PrintRelayStatus(relay_status);
			PrintTemperature();
		}
	}

	if (relay_timer.IsReady()) {
		relay_status = !relay_status;
		PrintRelayStatus(relay_status);
	}
	digitalWrite(PinsDefine::RELAY_PIN, relay_status);

}

void SensorSearch() {
	Serial.println("Search sensor...");
	sensor.begin();
	if (sensor.getDS18Count()) {
		PrintSensorAddress();
		sensor.setResolution(SensorResolution::TEMP_12_BIT);
		FirstReadTemperature();
		PrintTemperature();		
	}
	else {
		Serial.println("DS18B20 not found :(");
		sensor_research_timer.ResetTimer();
	}
}

void PrintSensorAddress() {
	uint8_t address[8];
	sensor.getAddress(address, 0);
	Serial.println("Sensor found! :)");
	Serial.print("DS18B20 address: ");
	for (uint8_t i = 0; i < 8; ++i) {
		Serial.print("0x");
		if (address[i] < 0x10) Serial.print("0");
		Serial.print(address[i], HEX);
	}
	Serial.println("");
}

void PrintRelayStatus(const bool status) {
	Serial.print("Relay is ");
	String s;
	s = status ? "on" : "off";
	Serial.println(s);
}

void FirstReadTemperature() {
	sensor.requestTemperatures();
	sensor_tick_timer.ResetTimer();
	TimeManager print_timer(Time::SEC_0_2);
	while (!sensor_tick_timer.IsReady()) {
		if (print_timer.IsReady()) {
			Serial.print('.');
		}
	}
	temperature = sensor.getTempCByIndex(0);
	sensor.requestTemperatures();
	sensor_tick_timer.ResetTimer();
	
}
void PrintTemperature() {
	Serial.println("");
	Serial.print("Temperature: ");
	Serial.println(temperature);
}
