#include "DallasTemperature.h"
#include "TimeManager.h"
#include "Trigger.h"

namespace Setup 
{
	enum PinsDefine {
		SENSOR_VCC = 2,
		ONE_WIRE_PIN = 7,
		RELAY_PIN = 8,
		LED = 13
	};
	namespace Timers {
		const Time LED_BLINK = Time::SEC_0_1;		// длительность свечения светодиода в режиме мигания
		const Time LED_WAIT_WITHOUT_SENSOR = Time::SEC_1; 	// задержка между мигинием светодиода когда нет датчика температуры
		const Time LED_WAIT_NOT_COLD = Time::SEC_5;	// задержка между мигинием светодиода когда есть датчик температуры и реле выключено
		const Time LED_WAIT_COLD = Time::SEC_0_3;	// задержка между мигинием светодиода когда холодно (сработало по temp_heating_on) и реле выключено
		const Time PRINT_DOT = Time::SEC_0_2;		// как часто печатать точки во время ожидания преобразования температуры
		const Time SENSOR_RESEARCH = Time::SEC_3;	// период поиска датчика, если был не обнаружен или отвалился
		const Time SENSOR_TICK = Time::SEC_1;		// период опроса датчика температуры
		const uint32_t RELAY = 1200000;				// период вкл/выкл реле (20 минут == 1200000)
	}

	const int SERIAL_SPEED = 9600;
	const float temp_heating_on = 15.0;
	const float temp_heating_off = 20.0;
}

enum SensorResolution {
	TEMP_9_BIT = 9,
	TEMP_10_BIT,
	TEMP_11_BIT,
	TEMP_12_BIT
};
OneWire one_wire(Setup::PinsDefine::ONE_WIRE_PIN);
DallasTemperature sensor(&one_wire);
float temperature = DEVICE_DISCONNECTED_C;

TimeManager sensor_research_timer(Setup::Timers::SENSOR_RESEARCH);
TimeManager sensor_tick_timer(Setup::Timers::SENSOR_TICK);
TimeManager relay_timer(Setup::Timers::RELAY);


void SensorSearch();
void PrintSensorAddress();
void PrintRelayStatus(const bool status);
void PrintTemperature();
void FirstReadTemperature();
void LedBlink(const Time wait);

void setup() {
	Serial.begin(Setup::SERIAL_SPEED);
	pinMode(Setup::PinsDefine::RELAY_PIN, OUTPUT);
	pinMode(Setup::PinsDefine::LED, OUTPUT);
	digitalWrite(Setup::PinsDefine::RELAY_PIN, HIGH);
	pinMode(Setup::PinsDefine::SENSOR_VCC, OUTPUT);
	digitalWrite(Setup::PinsDefine::SENSOR_VCC, HIGH);

	SensorSearch();	
	if (sensor.getDS18Count()) {
		digitalWrite(Setup::PinsDefine::LED, HIGH);
		delay(Time::SEC_0_2);
		digitalWrite(Setup::PinsDefine::LED, LOW);
	}
	else {
		uint8_t i = 0;
		while (i < 3) {
			digitalWrite(Setup::PinsDefine::LED, HIGH);
			delay(Time::SEC_0_1);
			digitalWrite(Setup::PinsDefine::LED, LOW);
			delay(Time::SEC_0_1);
			i++;
		}
		sensor_research_timer.ResetTimer();
	}
}

void loop() {
	static bool is_cold = false; // true - когда реле сработало по temp_heating_on, false - когда выключено по temp_heating_off
	static bool relay_status = is_cold;
	const bool is_sensor_enable = static_cast<int>(temperature) != DEVICE_DISCONNECTED_C;
	
	if (!is_sensor_enable && sensor_research_timer.IsReady()) {
		SensorSearch();
	}
	if (is_sensor_enable && sensor_tick_timer.IsReady()) {
		temperature = sensor.getTempCByIndex(0);
		sensor.requestTemperatures();
	}

// Логика работы с датчиком температуры
	if (is_sensor_enable) {
		const bool on_by_temp = temperature < Setup::temp_heating_on;
		static Trigger on_by_temp_trig;
		
		if (on_by_temp_trig.IsRisingFront(on_by_temp)) {
			relay_status = true;
			is_cold = true;
			Serial.println("Cold!");
			PrintRelayStatus(relay_status);
			PrintTemperature();
			relay_timer.ResetTimer();
		}
	
		const bool off_by_temp = temperature > Setup::temp_heating_off;
		if (off_by_temp) {
			is_cold = false;
			relay_status = false;
			relay_timer.ResetTimer();
		}
		static Trigger off_by_temp_trig;
		if (off_by_temp_trig.IsRisingFront(off_by_temp)) {
			PrintRelayStatus(relay_status);
			Serial.println("Not cold");
			PrintTemperature();
		}
	}
	else {
		is_cold = false;
	}

// Работа реле
	if (relay_timer.IsReady()) {
		if (!is_sensor_enable || (is_sensor_enable && is_cold)) {
			relay_status = !relay_status;
		}
		PrintRelayStatus(relay_status);
		PrintTemperature();
	}
	digitalWrite(Setup::PinsDefine::RELAY_PIN, !relay_status);
	
// Работа светодиода
	uint8_t led_mode = 0;
	if (relay_status)
		led_mode = 0;
	else {
		if (is_sensor_enable)
			if (is_cold) 
				led_mode = 3;
			else 
				led_mode = 2;
		else {
			led_mode = 1;
		}
	}

	switch (led_mode) {
	case 0:
		digitalWrite(Setup::PinsDefine::LED, relay_status);
		break;
	case 1:
		LedBlink(Setup::Timers::LED_WAIT_WITHOUT_SENSOR);
		break;
	case 2:
		LedBlink(Setup::Timers::LED_WAIT_NOT_COLD);
		break;
	case 3:
		LedBlink(Setup::Timers::LED_WAIT_COLD);
		break;
	}
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
	String addr_str;
	for (uint8_t i = 0; i < 8; ++i) {
		// делаем адрес в строковом представлении
		addr_str += address[i] < 16 ? "00" : String(address[i], HEX);
	}
	Serial.println(addr_str);
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
	TimeManager print_timer(Setup::Timers::PRINT_DOT);
	while (!sensor_tick_timer.IsReady()) {
		if (print_timer.IsReady()) {
			Serial.print('.');
		}
	}
	Serial.println("");
	temperature = sensor.getTempCByIndex(0);
	sensor.requestTemperatures();
	sensor_tick_timer.ResetTimer();
	
}
void PrintTemperature() {
	Serial.print("Temperature: ");
	Serial.println(temperature);
}

void LedBlink(const Time wait) {
	static TimeManager blink_timer(Setup::Timers::LED_BLINK);
	static TimeManager blink_wait_timer(wait);

	static Time last_wait = wait;
	if (last_wait != wait) {
		blink_wait_timer.SetPeriod(wait);
		last_wait = wait;
	}

	static bool is_blink = true;
	if (is_blink && blink_timer.IsReady()) {
		is_blink = false;
	}
	if (!is_blink && blink_wait_timer.IsReady()) {
		is_blink = true;
		blink_timer.ResetTimer();
	}
	digitalWrite(Setup::PinsDefine::LED, is_blink);
}