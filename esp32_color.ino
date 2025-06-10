#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "password";
const char* password = "PassKey1234";

const int led1 = 21;  // Red LED
const int led2 = 22;  // Green LED  
const int led3 = 23;  // Blue LED


// PWM properties
const int freq = 5000;
const int resolution = 8;
WebServer server(80);

void handleColor() {
  if (server.hasArg("color")) {
    String color = server.arg("color"); // expects "255,0,0" for red for example
    Serial.println("Received color: " + color);
    parseAndSetColor(color);
    // Parse color and update your RGB LED here
    server.send(200, "text/plain", "Color updated");
  } else {
    server.send(400, "text/plain", "No color argument");
  }
}

    void parseAndSetColor(String colorData) {
      // Parse "R,G,B" format
      int firstComma = colorData.indexOf(',');
      int secondComma = colorData.indexOf(',', firstComma + 1);
      
      if (firstComma > 0 && secondComma > firstComma) {
        int r = colorData.substring(0, firstComma).toInt();
        int g = colorData.substring(firstComma + 1, secondComma).toInt();
        int b = colorData.substring(secondComma + 1).toInt();
        
        // Constrain values to 0-255
        r = constrain(r, 0, 255);
        g = constrain(g, 0, 255);
        b = constrain(b, 0, 255);
        
        Serial.printf("Setting RGB: %d, %d, %d\n", r, g, b);
        
        // Set LED brightness using PWM
        ledcWrite(led1, r);
        ledcWrite(led2, g);
        ledcWrite(led3, b);
      }
    }

void setup() {
  Serial.begin(115200);

  // Configure LED PWM channels (newer ESP32 core version)
  ledcAttach(led1, freq, resolution);
  ledcAttach(led2, freq, resolution);
  ledcAttach(led3, freq, resolution);
  
  // Turn off all LEDs initially
  ledcWrite(led1, 0);
  ledcWrite(led2, 0);
  ledcWrite(led3, 0);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected, IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/color", HTTP_POST, handleColor);
  server.begin();
}

void loop() {
  server.handleClient();
}
