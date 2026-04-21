#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "config.h"

#define DHTPIN 4
#define DHTTYPE DHT11
#define SOIL_PIN 34
#define PIR_PIN 27
#define RELAY_PIN 26

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

int soilThreshold = 40;

BlynkTimer timer;

void sendSensorData() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int soil = analogRead(SOIL_PIN);
  soil = map(soil, 0, 4095, 100, 0);
  int motion = digitalRead(PIR_PIN);

  Blynk.virtualWrite(V0, temp);
  Blynk.virtualWrite(V1, hum);
  Blynk.virtualWrite(V3, soil);
  Blynk.virtualWrite(V5, motion);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temp);
  lcd.print(" H:");
  lcd.print(hum);

  lcd.setCursor(0, 1);
  lcd.print("Soil:");
  lcd.print(soil);

  if (soil < soilThreshold) {
    digitalWrite(RELAY_PIN, LOW);
    Blynk.virtualWrite(V12, 1);
  } else {
    digitalWrite(RELAY_PIN, HIGH);
    Blynk.virtualWrite(V12, 0);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, HIGH);

  dht.begin();
  lcd.init();
  lcd.backlight();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASS);

  timer.setInterval(2000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
}
