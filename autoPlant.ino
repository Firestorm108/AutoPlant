#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>
#include <Adafruit_SHT4x.h>
#define LIGHT_RELAY 5
#define FAN_RELAY 6
#define WATER_RELAY 8
#define ENCODER_CLK 2
#define ENCODER_DT 3
#define ENCODER_BTN 12
LiquidCrystal_I2C lcd(0x27, 16, 2);
Encoder knob(ENCODER_CLK, ENCODER_DT);
Adafruit_SHT4x sht40 = Adafruit_SHT4x();
int lightHours = 12;
int waterMinutes = 5;
bool lightOn = false;
unsigned long lightStartTime = 0;
unsigned long waterStartTime = 0;
unsigned long lastLightCycle = 0;
unsigned long lastWaterCycle = 0;
const unsigned long oneDay = 24UL * 60UL * 60UL * 1000UL; 
void setup() {
 pinMode(LIGHT_RELAY, OUTPUT);
 pinMode(FAN_RELAY, OUTPUT);
 pinMode(WATER_RELAY, OUTPUT);
 pinMode(ENCODER_BTN, INPUT_PULLUP);
 digitalWrite(LIGHT_RELAY, LOW);
 digitalWrite(WATER_RELAY, LOW);
 digitalWrite(FAN_RELAY, HIGH); 
 lcd.init();
 lcd.backlight();
 Wire.begin();

 if (!sht40.begin()) {
   lcd.clear();
   lcd.print("SHT40 not found");
   delay(3000);
 }

 lcd.clear();
 lcd.print("Set Light Hours:");
 selectValue(lightHours, 0, 24);


 lcd.clear();
 lcd.print("Set Water Min:");
 selectValue(waterMinutes, 0, 60);


 lastLightCycle = millis();
 lastWaterCycle = millis();
}


void loop() {
 unsigned long now = millis();


 if (now - lastLightCycle >= oneDay) {
   lastLightCycle = now;
   lightStartTime = now;
   digitalWrite(LIGHT_RELAY, HIGH);
   lightOn = true;
 }


 if (lightOn && (now - lightStartTime >= (unsigned long)lightHours * 60UL * 60UL * 1000UL)) {
   digitalWrite(LIGHT_RELAY, LOW);
   lightOn = false;
 }


 if (now - lastWaterCycle >= oneDay) {
   lastWaterCycle = now;
   waterStartTime = now;
   digitalWrite(WATER_RELAY, HIGH);
 }


 if (digitalRead(WATER_RELAY) == HIGH &&
     (now - waterStartTime >= (unsigned long)waterMinutes * 60UL * 1000UL)) {
   digitalWrite(WATER_RELAY, LOW);
 }


 sensors_event_t humidity, temp;
 sht40.getEvent(&humidity, &temp);


 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("T:");
 lcd.print(temp.temperature, 1);
 lcd.print("C H:");
 lcd.print(humidity.relative_humidity, 0);
 lcd.print("%");


 lcd.setCursor(0, 1);
 lcd.print("L:");
 lcd.print(lightHours);
 lcd.print("h W:");
 lcd.print(waterMinutes);
 lcd.print("m");


 delay(1000);
}

void selectValue(int &value, int minVal, int maxVal) {
 long lastPos = knob.read();
 while (true) {
   long newPos = knob.read();
   if (newPos != lastPos) {
     value += (newPos - lastPos) / 4; 
     if (value < minVal) value = minVal;
     if (value > maxVal) value = maxVal;
     lastPos = newPos;


     lcd.setCursor(0, 1);
     lcd.print("Value: ");
     lcd.print(value);
     lcd.print("    ");
   }


   if (digitalRead(ENCODER_BTN) == LOW) {
     delay(200); // debounce
     while (digitalRead(ENCODER_BTN) == LOW); 
     break;
   }
 }
}



