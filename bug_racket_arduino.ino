#include <Arduino_JSON.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>

// Network credentials
const char* ssid = "FarDream";
const char* password = "fardream526";

// Backend server details
const char* serverAddress = "bugracket.nn.r.appspot.com";
int port = 443;

// Button setup
const int buttonPin = 13;
int buttonState = HIGH;
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
bool connectionSuccess = false; // Flag to track if initial connection was successful

WiFiSSLClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);
  while (!Serial);

  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");

  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect to WiFi");
    return;
  }

  delay(1000);

  // Send initial data to backend
  String plainTextData = "80:DE:82:60:62:EC  "; // Initial data for checking connection
  sendToBackend(plainTextData, "text/plain", true); // Pass true for initial connection check
}

void loop() {
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW && connectionSuccess) { // Only send if initial connection was successful
        sendToBackend("button_push  ", "text/plain", false);
      }
    }
  }

  lastButtonState = reading;
}

void sendToBackend(String data, String contentType, bool isInitialConnection) {
  client.beginRequest();
  client.post("/arduino/mac");
  client.sendHeader("Content-Type", contentType);
  client.sendHeader("Content-Length", data.length());
  client.println(); // End of headers
  client.print(data);
  client.endRequest();

  Serial.println("Data sent, waiting for response...");
  int statusCode = client.responseStatusCode();
  Serial.print("Status code: ");
  Serial.println(statusCode);

  if (statusCode == 200) {
    if (isInitialConnection) {
      Serial.println("Connection success");
      connectionSuccess = true; // Set flag to true on successful initial connection
    }
  } else {
    if (isInitialConnection) {
      Serial.println("Backend connection error");
      connectionSuccess = false; // Ensure flag remains false on failed initial connection
    }
  }

  String response = client.responseBody();
  Serial.print("Response: ");
  Serial.println(response);
}
