#include <ESP8266WiFi.h>

const char* ssid = "narzo";         // Your Wi-Fi SSID
const char* password = "123456789"; // Your Wi-Fi Password

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");

  // Wait until the ESP8266 is connected to Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }

  // Once connected, print the IP address
  Serial.println("Connected to WiFi");
  Serial.print("ESP8266 IP Address: ");
  Serial.println(WiFi.localIP()); // Print the local IP address
}

void loop() {
  // Your code here
}
