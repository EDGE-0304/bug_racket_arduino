#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoBLE.h>

// Define the button pin and debounce variables
const int buttonPin = 13;
bool lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
int buttonPresses = 0;

// BLE Service and Characteristic UUIDs
BLEService wifiService("12345678-1234-1234-1234-123456789012");
BLECharacteristic wifiSSIDCharacteristic("abcd1234-1234-1234-1234-123456789012", BLEWrite, 32);
BLECharacteristic wifiPassCharacteristic("abcd5678-1234-1234-1234-123456789012", BLEWrite, 64);

// WiFi Credentials and Server Information
char ssid[33]; // SSID array including null terminator
char pass[65]; // Password array including null terminator
const char server[] = "bugracket.nn.r.appspot.com"; // Backend server address
WiFiClient client;

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);

  // Initialize BLE
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (true);
  }

  BLE.setLocalName("Nano33IoT");
  BLE.setAdvertisedService(wifiService);
  wifiService.addCharacteristic(wifiSSIDCharacteristic);
  wifiService.addCharacteristic(wifiPassCharacteristic);
  BLE.addService(wifiService);
  BLE.advertise();
  Serial.println("BLE advertising started");
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.println("Connected to central device");

    while (central.connected()) {
      if (wifiSSIDCharacteristic.written() && wifiPassCharacteristic.written()) {
        wifiSSIDCharacteristic.readValue(ssid, sizeof(ssid));
        wifiPassCharacteristic.readValue(pass, sizeof(pass));
        Serial.print("SSID received: ");
        Serial.println(ssid);
        Serial.print("Password received: ");
        Serial.println(pass);

        // Attempt to connect to WiFi after receiving credentials
        connectToWiFi();
      }
    }
  }

  // Handle button press for backend communication
  handleButtonPress();
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi SSID: ");
  Serial.println(ssid);
  int status = WiFi.begin(ssid, pass);

  if (status != WL_CONNECTED) {
    Serial.println("Failed to connect to WiFi. Please check credentials.");
    return; // Exit if connection is unsuccessful
  }

  Serial.println("Connected to WiFi");
  connectToBackend(); // Proceed to connect to backend after successful WiFi connection
}

void connectToBackend() {
  Serial.println("Connecting to backend server...");
  if (client.connect(server, 443)) { // HTTP port
    Serial.println("Connected to backend server");

    // Placeholder for backend communication, e.g., HTTP GET request
    client.println("GET /api/hello HTTP/1.1");
    client.println("Host: " + String(server));
    client.println("Connection: close");
    client.println(); // End of HTTP request
  } else {
    Serial.println("Failed to connect to backend server");
  }
}

void handleButtonPress() {
  bool reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != lastButtonState) {
      lastButtonState = reading;
      if (reading == HIGH) {
        buttonPresses++;
        Serial.print("Button pressed: ");
        Serial.println(buttonPresses);

        // Send a message to the backend upon button press
        if (WiFi.status() == WL_CONNECTED) {
          sendButtonPressToBackend();
        } else {
          Serial.println("WiFi is not connected. Cannot send button press to backend.");
        }
      }
    }
  }
}

void sendButtonPressToBackend() {
  if (client.connect(server, 443)) {
    client.print("GET /api/buttonPress?count=");
    client.print(buttonPresses);
    client.println(" HTTP/1.1");
    client.println("Host: " + String(server));
    client.println("Connection: close");
    client.println();
    Serial.println("Button press data sent to backend");
  } else {
    Serial.println("Failed to connect to backend for sending button press data");
  }
} // Correctly closing the sendButtonPressToBackend function
