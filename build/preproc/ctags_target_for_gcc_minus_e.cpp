# 1 "D:\\home_shurik\\Arduino\\Ruler\\Ruler.ino"
# 2 "D:\\home_shurik\\Arduino\\Ruler\\Ruler.ino" 2
# 3 "D:\\home_shurik\\Arduino\\Ruler\\Ruler.ino" 2
# 4 "D:\\home_shurik\\Arduino\\Ruler\\Ruler.ino" 2

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

void ButtonTick();
void MeasureMode();
void SetReverseMode();

void setup() {
 // Serial.begin(9600);

 lcd.begin();
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print((reinterpret_cast<const __FlashStringHelper *>(
# 34 "D:\\home_shurik\\Arduino\\Ruler\\Ruler.ino" 3
          (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 34 "D:\\home_shurik\\Arduino\\Ruler\\Ruler.ino"
          "Rotary Ruler"
# 34 "D:\\home_shurik\\Arduino\\Ruler\\Ruler.ino" 3
          ); &__c[0];}))
# 34 "D:\\home_shurik\\Arduino\\Ruler\\Ruler.ino"
          )));

 ruler.setDiameter(diameter_mm);
 ruler.setReverse(true);

 currentScreen = MenuItem::MEASURE;
}

void loop() {
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
}

void ButtonTick() {
 // TODO: Обработать нажатие кнопки
}

void MeasureMode() {
 const bool isInc = ruler.isIncremented();
 const bool isDec = ruler.isDecremented();
 lcd.setCursor(15, 0);
 static TimeManager dirTimer(static_cast<uint32_t>(Time::SEC_0_5));
 if (isInc || isDec) {
  dirTimer.ResetTimer();
  if (isInc) {
   lcd.print('>');
  }
  else if (isDec) {
   lcd.print('<');
  }
 }
 else if (dirTimer.IsReady()) {
  lcd.print(' ');
 }

 if (ruler.isDistanseChanged()) {
  lcd.setCursor(0, 0);
  lcd.print((reinterpret_cast<const __FlashStringHelper *>(
# 80 "D:\\home_shurik\\Arduino\\Ruler\\Ruler.ino" 3
           (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 80 "D:\\home_shurik\\Arduino\\Ruler\\Ruler.ino"
           "Rotary Ruler"
# 80 "D:\\home_shurik\\Arduino\\Ruler\\Ruler.ino" 3
           ); &__c[0];}))
# 80 "D:\\home_shurik\\Arduino\\Ruler\\Ruler.ino"
           )));

  lcd.setCursor(0, 1);
  lcd.print(ruler.getDistance(), 2);
  lcd.print((reinterpret_cast<const __FlashStringHelper *>(
# 84 "D:\\home_shurik\\Arduino\\Ruler\\Ruler.ino" 3
           (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 84 "D:\\home_shurik\\Arduino\\Ruler\\Ruler.ino"
           " mm             "
# 84 "D:\\home_shurik\\Arduino\\Ruler\\Ruler.ino" 3
           ); &__c[0];}))
# 84 "D:\\home_shurik\\Arduino\\Ruler\\Ruler.ino"
           )));

  // Serial.print(ruler.getDistance(), 3);
  // Serial.print(" mm, inc: "); Serial.print(isInc);
  // Serial.print(", dec: "); Serial.println(isDec);
 }
}

void SetReverseMode() {
 // TODO: Вывод информации на экран
 const bool isInc = ruler.isIncremented();
 const bool isDec = ruler.isDecremented();
 if (ruler.isDistanseChanged() && (isInc || isDec )) {
  ruler.setReverse(!ruler.getReverseMode());
 }
}
