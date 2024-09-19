// Replace Blynk credentials
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
#include <ESP8266WebServer.h>

#define DPIN 4        // Pin to connect DHT sensor (GPIO number) D2
#define DTYPE DHT11   // Define DHT 11 or DHT22 sensor type
#define MAX_FIELD_TEMP 40
#define MIN_FIELD_HUMIDITY 45
#define BLYNK_PRINT Serial
#define RAIN_SENSOR_PIN A0
#define RAIN_SENSOR_THRESHOLD 500 // Adjust this value based on your sensor

// WiFi credentials
const char* ssid = "narzo";
const char* password = "123456789";

// ThingSpeak settings
unsigned long myChannelNumber = 2656641;  // Replace with your ThingSpeak channel number
const char* myWriteAPIKey = "U1Y9O163FS4BRZWU";  // Replace with your ThingSpeak Write API Key

// Create an instance of the web server
ESP8266WebServer server(80);

// Variables to store received data
String receivedTemp = "";
String receivedHumidity = "";
String receivedRainChance = "";

DHT dht(DPIN, DTYPE);
WiFiClient client;

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("ESP8266 IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  // Initialize DHT sensor
  dht.begin();

  // Initialize ThingSpeak
  ThingSpeak.begin(client);

  // Start the web server
  server.begin();
  
  // Define the endpoint to receive data
  server.on("/update", HTTP_GET, []() {
    if (server.hasArg("temp") && server.hasArg("humidity") && server.hasArg("rainChance")) {
      // Store the received data
      receivedTemp = server.arg("temp");
      receivedHumidity = server.arg("humidity");
      receivedRainChance = server.arg("rainChance");

      // Print the received data to the serial monitor
      Serial.println("Received data:");
      Serial.println("Temperature: " + receivedTemp);
      Serial.println("Humidity: " + receivedHumidity);
      Serial.println("Chance of Rain: " + receivedRainChance);

      server.send(200, "text/plain", "Data received");
    } else {
      server.send(400, "text/plain", "Invalid request");
    }
  });

}

void loop() {
  // Handle web server requests
  server.handleClient();

  // Read temperature and humidity from DHT sensor
  float sensorTemp = dht.readTemperature();  // Sensor temperature in Celsius
  float sensorHumidity = dht.readHumidity(); // Sensor humidity

  // Print sensor temperature and humidity to the serial monitor
  Serial.print("Sensor Temp: ");
  Serial.print(sensorTemp);
  Serial.print(" C, Sensor Hum: ");
  Serial.print(sensorHumidity);
  Serial.println("%");

  // Read rain sensor value
  int rainValue = analogRead(RAIN_SENSOR_PIN);
  Serial.print("Sensor Rain value: ");
  Serial.println(rainValue);

  // Blynk notifications based on sensor thresholds
  if (sensorTemp > MAX_FIELD_TEMP) {
    Blynk.logEvent("maxfieldtemp");  // Notify if field temperature is too high
  }
  if (sensorHumidity < MIN_FIELD_HUMIDITY) {
    Blynk.logEvent("minfieldhumidity");  // Notify if humidity is too low
  }
  if (rainValue < RAIN_SENSOR_THRESHOLD) {
    Blynk.logEvent("rainalert");  // Notify if rain is detected
  }

  // Update Blynk virtual pins for sensor temperature, humidity, and rain
  Blynk.virtualWrite(V0, sensorTemp);
  Blynk.virtualWrite(V1, sensorHumidity);
  Blynk.virtualWrite(V2, rainValue);

  // Compare sensor data with web data (receivedTemp, receivedHumidity, receivedRainChance)
  if (receivedTemp != "" && receivedHumidity != "" && receivedRainChance != "") {
    // Convert web data to float for comparison
    float webTemp = receivedTemp.toFloat();
    float webHumidity = receivedHumidity.toFloat();
    float webRainChance = receivedRainChance.toFloat();

    // Print web data to the serial monitor
    Serial.println("Web Temperature: " + String(webTemp));
    Serial.println("Web Humidity: " + String(webHumidity));
    Serial.println("Web Chance of Rain: " + String(webRainChance));

    // Send web data to Blynk virtual pins V3, V4, V5
    Blynk.virtualWrite(V3, webTemp);       // Send web temperature to virtual pin V3
    Blynk.virtualWrite(V4, webHumidity);   // Send web humidity to virtual pin V4
    Blynk.virtualWrite(V5, webRainChance); // Send web rain chance to virtual pin V5

    // Alert: If sensor temperature is significantly higher than web temperature
    if (sensorTemp > webTemp + 5) {
      Blynk.logEvent("hot_alert", "Scorching Hot! Sensor shows much higher temperature than expected!");  // Custom Blynk event
    }

    // Alert: If sensor humidity is significantly lower than web humidity
    if (sensorHumidity < webHumidity - 10) {
      Blynk.logEvent("dry_alert", "Dry Spell! Sensor humidity is much lower than forecasted.");
    }

    // Alert: If rain is detected by sensor but web shows low chance of rain
    if (rainValue < RAIN_SENSOR_THRESHOLD && webRainChance < 30) {
      Blynk.logEvent("rain_surprise", "Surprise Shower! Rain detected, but the forecast says otherwise.");
    }

    // Alert: If web data shows high chance of rain but no rain detected by sensor
    if (rainValue > RAIN_SENSOR_THRESHOLD && webRainChance > 80) {
      Blynk.logEvent("rain_tease", "Rain Tease! High chance of rain, but skies are clear.");
    }
  }

  // Send data to ThingSpeak
  ThingSpeak.setField(1, sensorTemp);  // Field 1 = Temperature
  ThingSpeak.setField(3, sensorHumidity);  // Field 3 = Humidity
  int statusCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
}
  


