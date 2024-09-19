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

#include "ThingSpeak.h"

#define DPIN 4        //Pin to connect DHT sensor (GPIO number) D2
#define DTYPE DHT11   // Define DHT 11 or DHT22 sensor type
#define MAX_FIELD_TEMP 40
#define MIN_FIELD_HUMIDITY 45

#define BLYNK_PRINT Serial

#define RAIN_SENSOR_PIN A0
#define RAIN_SENSOR_THRESHOLD 500 // adjust this value based on your sensor

char ssid[] = "narzo";
char pass[] = "123456789";

// ThingSpeak settings
unsigned long myChannelNumber = 2656641;  // Replace with your ThingSpeak channel number
const char* myWriteAPIKey = "U1Y9O163FS4BRZWU";  // Replace with your ThingSpeak Write API Key

DHT dht(DPIN,DTYPE);

WiFiClient client;

void setup()
{
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  dht.begin();

  Serial.print("ESP8266 IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize ThingSpeak
  ThingSpeak.begin(client);
  
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

    if (tc > MAX_FIELD_TEMP) {
      Blynk.logEvent("maxfieldtemp");   //Blynk notification maximum field temperature
    }
    if (hu < MIN_FIELD_HUMIDITY) {
      Blynk.logEvent("minfieldhumidity");   // Blynk notification minimum field humidity
    }

  int rainValue = analogRead(RAIN_SENSOR_PIN);
  Serial.print("Rain sensor value: ");
  Serial.println(rainValue);

  if (rainValue < RAIN_SENSOR_THRESHOLD) {
    Blynk.logEvent("rainalert");    // Blynk notification rain alert
  }

  Blynk.virtualWrite(V0, tc);
  Blynk.virtualWrite(V1, hu);

  Blynk.run();

  // Set the values to be sent to ThingSpeak
  ThingSpeak.setField(1, tc);  // Field 1 = Temperature
  ThingSpeak.setField(3, hu);     // Field 3 = Humidity

  // Send the data to ThingSpeak
  int statusCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  

  // Wait for 2 seconds before sending the next update
  delay(2000);

}