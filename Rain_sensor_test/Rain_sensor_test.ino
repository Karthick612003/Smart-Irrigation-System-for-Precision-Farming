//replace blynk credentials
#define BLYNK_TEMPLATE_ID "TMPL37P25vRkN"
#define BLYNK_TEMPLATE_NAME "Temp and Hum with rain alert"
#define BLYNK_AUTH_TOKEN "JzjBxiWAXLv9FNezwFqYltjP5CgR8Pkt"

#include <SPI.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <DHTStable.h>
#include <Adafruit_Sensor.h>
#include "DHT.h"

#define DPIN 4        //Pin to connect DHT sensor (GPIO number) D2
#define DTYPE DHT11   // Define DHT 11 or DHT22 sensor type

#define BLYNK_PRINT Serial

#define RAIN_SENSOR_PIN A0
#define RAIN_SENSOR_THRESHOLD 500 // adjust this value based on your sensor

char ssid[] = "narzo";
char pass[] = "123456789";

DHT dht(DPIN,DTYPE);

void setup()
{
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  dht.begin();
}

void loop()
{

    float tc = dht.readTemperature();  //Read temperature in C
    float hu = dht.readHumidity();          //Read Humidity

    Serial.print("Temp: ");
    Serial.print(tc);
    Serial.print(" C, Hum: ");
    Serial.print(hu);
    Serial.println("%");

  int sensorValue = analogRead(RAIN_SENSOR_PIN);
  Serial.print("Rain sensor value: ");
  Serial.println(sensorValue);

  if (sensorValue < RAIN_SENSOR_THRESHOLD) {
    Blynk.logEvent("rainalert");
  }

  Blynk.virtualWrite(V0, tc);
  Blynk.virtualWrite(V1, hu);

  Blynk.run();
  delay(2000);
}