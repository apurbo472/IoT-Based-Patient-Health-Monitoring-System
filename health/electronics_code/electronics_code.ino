#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal.h>
#include <DHT.h>

#define DHTPIN 7
#define DHTTYPE DHT11
#define PULSEPIN 3  // MUST move Pulse Generator wire to Pin 3 in Proteus
#define ACC_X A0
#define ACC_Y A1
#define ACC_Z A2

DHT dht(DHTPIN, DHTTYPE);
// Pins: RS, EN, D4, D5, D6, D7. 
// Note: We moved D6 to Pin 6 to keep Pin 3 free for the Pulse.
LiquidCrystal lcd(12, 11, 5, 4, 6, 2); 
RTC_DS1307 rtc;

// Global Variables
volatile unsigned long lastBeatTime = 0;
volatile int bpm = 0;
unsigned long lastScreenUpdate = 0;
int screenState = 0; // 0=Time, 1=Health, 2=Movement

// Interrupt function for accurate BPM
void countBPM() {
  unsigned long now = millis();
  unsigned long interval = now - lastBeatTime;
  if (interval > 300) { // Noise filter
    bpm = 60000 / interval;
    lastBeatTime = now;
  }
}

void setup() {
  lcd.begin(16, 2);
  pinMode(PULSEPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PULSEPIN), countBPM, RISING);

  dht.begin();
  Wire.begin();
  rtc.begin();
  
  lcd.print("System Loading...");
  delay(1000);
}

void loop() {
  // Rotate screens every 2 seconds without using delay()
  if (millis() - lastScreenUpdate > 2000) {
    lastScreenUpdate = millis();
    lcd.clear();

    if (screenState == 0) {
      // --- SCREEN 1: TIME AND DATE ---
      DateTime now = rtc.now();
      lcd.setCursor(0, 0);
      lcd.print("Time: ");
      if(now.hour() < 10) lcd.print("0"); lcd.print(now.hour());
      lcd.print(":");
      if(now.minute() < 10) lcd.print("0"); lcd.print(now.minute());

      lcd.setCursor(0, 1);
      lcd.print("Date: ");
      lcd.print(now.day()); lcd.print("/");
      lcd.print(now.month()); lcd.print("/");
      lcd.print(now.year());
      
      screenState = 1; // Prepare for Health screen
    } 
    else if (screenState == 1) {
      // --- SCREEN 2: HEALTH (BPM, TEMP, HUM) ---
      float t = dht.readTemperature();
      float h = dht.readHumidity();

      lcd.setCursor(0, 0);
      lcd.print("BPM:"); lcd.print(bpm);
      lcd.setCursor(9, 0);
      lcd.print("T:"); 
      lcd.print(isnan(t) ? 0 : (int)t); lcd.print("C");

      lcd.setCursor(0, 1);
      lcd.print("Humidity: "); 
      lcd.print(isnan(h) ? 0 : (int)h); lcd.print("%");
      
      screenState = 2; // Prepare for Accelerometer screen
    } 
    else {
      // --- SCREEN 3: ACCELEROMETER (X, Y, Z) ---
      lcd.setCursor(0, 0);
      lcd.print("X:"); lcd.print(analogRead(ACC_X));
      lcd.print(" Y:"); lcd.print(analogRead(ACC_Y));
      lcd.setCursor(0, 1);
      lcd.print("Z:"); lcd.print(analogRead(ACC_Z));
      lcd.print("  [SCAN]");

      screenState = 0; // Loop back to Time screen
    }
  }
}