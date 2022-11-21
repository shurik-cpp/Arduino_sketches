
void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A0));
  pinMode(13, OUTPUT);
}

void loop() {
  static uint32_t counter_millis = millis();
  static uint32_t every_one_seconds_millis = millis();
  static bool blink = true;
  const uint8_t LED_PIN = 13;
  static int random_delay = analogRead(A0);
  uint8_t random_case = random(4);//(255);
  
  if (millis() - counter_millis >= random_delay) {
    counter_millis = millis();
    do {
      random_delay = analogRead(A0) * random(7);
    } while (random_delay == 0);
    
    Serial.print("millis() = ");
    Serial.print(counter_millis);
    Serial.print(", delay = ");
    Serial.print(random_delay);
    Serial.print(", case = ");
    Serial.println(random_case);
    
    digitalWrite(LED_PIN, blink);
    blink = !blink;
  }

  if (millis() - every_one_seconds_millis >= 1000) {
    every_one_seconds_millis = millis();
    Serial.print("uptime seconds = ");
    Serial.println(every_one_seconds_millis / 1000);
  }
}
