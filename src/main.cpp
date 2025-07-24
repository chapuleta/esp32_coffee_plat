#include <Arduino.h>

#include <WiFi.h>

#include <Firebase.h>

#include <Adafruit_GFX.h>

#include <Adafruit_SSD1306.h>

#include <NTPClient.h>

#include <WiFiUdp.h>



#include "config.h"



// Firebase objects

FirebaseData fbdo;

FirebaseAuth auth;

FirebaseConfig config;



// OLED Display object

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



// NTP Client

WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP, "pool.ntp.org", 3 * 3600); // GMT-3 for Brazil



// Function to print messages on OLED

void printOLED(const String& message, int textSize = 1) {

display.clearDisplay();

display.setTextSize(textSize);

display.setTextColor(SSD1306_WHITE);

display.setCursor(0, 0);

display.println(message);

display.display();

}



// Firebase stream callback function

void streamCallback(FirebaseStream data) {

Serial.printf("Stream path: %s, Type: %s, Event: %s\n", data.dataPath().c_str(), data.dataType().c_str(), data.eventType().c_str());



if (data.dataType() == "json") {

FirebaseJson json;

FirebaseJsonData result;



// Parse the JSON string from the stream

json.setJsonData(data.stringData());



// Get the status field

json.get(result, "status");



if (result.success) {

String status = result.stringValue;

Serial.printf("Status: %s\n", status.c_str());



if (status == "approved") {

Serial.println("Donation approved! Updating OLED and Firebase status...");

printOLED("Obrigado!", 2); // Display "Obrigado!" on OLED



// Update Firebase status to "processed"

Firebase.RTDB.setString(&fbdo, "/payment_status/status", "processed");

if (fbdo.httpCode() == FIREBASE_ERROR_HTTP_CODE_OK) {

Serial.println("Firebase status updated to processed.");

} else {

Serial.printf("Failed to update Firebase status: %s\n", fbdo.errorReason().c_str());

}

}

}

}

}



// Firebase stream timeout callback function

void streamTimeoutCallback(bool timeout) {

if (timeout) {

Serial.println("Stream timeout, re-establishing...");

}

Serial.println(fbdo.errorReason());

}



void setup() {

Serial.begin(115200);



// Connect to Wi-Fi

Serial.print("Connecting to Wi-Fi...");

WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

while (WiFi.status() != WL_CONNECTED) {

delay(500);

Serial.print(".");

}

Serial.println("\nWiFi connected.");

Serial.print("IP Address: ");

Serial.println(WiFi.localIP());



// Initialize and synchronize time with NTP server

Serial.println("Synchronizing time with NTP server...");

timeClient.begin();

while(!timeClient.update()) {

timeClient.forceUpdate();

Serial.print(".");

delay(500);

}

Serial.println("\nTime synchronized.");

Serial.println(timeClient.getFormattedTime());



// Initialize OLED display

if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64

Serial.println(F("SSD1306 allocation failed"));

for (;;); // Don't proceed, loop forever

}

display.display();

delay(2000); // Pause for 2 seconds

printOLED("Connecting...", 1);



// Firebase configuration

config.host = FIREBASE_HOST;

config.signer.tokens.legacy_token = FIREBASE_AUTH;



Firebase.begin(&config, &auth);

Firebase.reconnectWiFi(true);



// Set the size of the FirebaseData object's internal buffers

fbdo.setBSSLBufferSize(4096, 1024); // For FirebaseData object

// Assign the callback functions to the Firebase object

Firebase.RTDB.setStreamCallback(&fbdo, streamCallback, streamTimeoutCallback);



// Begin stream on the payment_status node

if (!Firebase.RTDB.beginStream(&fbdo, "/payment_status")) {

Serial.printf("Stream error: %s\n", fbdo.errorReason().c_str());

}

}



void loop() {

// Firebase.loop() should be called in the loop() function for stream processing

// and other background tasks.

if (Firebase.ready()) {

// Keep the stream alive

} else {

Serial.println("Firebase not connected. Reconnecting...");

// Reconnect logic is handled by Firebase.reconnectWiFi(true) and Firebase.begin()

// No explicit reconnect needed here unless you want custom logic.

}

delay(10); // Small delay to prevent watchdog timer issues

}