// main.cpp - ESP32 µDiag Client
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>

// === Configuration ===
#define OLED_SDA 21
#define OLED_SCL 22
#define UART_BAUD 115200
#define WIFI_SSID "your_wifi"
#define WIFI_PASS "your_pass"
#define FIREBASE_HOST "your-project.firebaseio.com"
#define FIREBASE_AUTH "your_secret"

// === OLED Setup ===
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// === WiFi & MQTT ===
WiFiClient espClient;
PubSubClient mqttClient(espClient);
const char* mqtt_server = "broker.hivemq.com";
unsigned long lastUpload = 0;

// === Fault Logging ===
struct FaultEvent {
    uint8_t flags;
    unsigned long timestamp;
};
#define LOG_SIZE 32
FaultEvent faultLog[LOG_SIZE];
int logIndex = 0;
bool logFull = false;

// === UART Buffer ===
uint8_t faultFlags = 0;

void setup() {
    Serial.begin(UART_BAUD);
    oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(SSD1306_WHITE);
    connectWiFi();
    mqttClient.setServer(mqtt_server, 1883);
    displayBootScreen();
}

void connectWiFi() {
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected!");
}

void loop() {
    readUART();
    updateDisplay();
    if (millis() - lastUpload > 5000) {
        uploadToCloud();
        lastUpload = millis();
    }
    delay(100);
}

void readUART() {
    if (Serial.available()) {
        faultFlags = Serial.read();
        if (faultFlags != 0) {
            FaultEvent ev = {faultFlags, millis()};
            faultLog[logIndex] = ev;
            logIndex = (logIndex + 1) % LOG_SIZE;
            if (logIndex == 0) logFull = true;
            Serial.printf("Fault Detected: 0x%02X\n", faultFlags);
        }
    }
}

void updateDisplay() {
    oled.clearDisplay();
    oled.setCursor(0, 0);
    oled.println("µDiag - Live Faults");
    oled.println("-------------------");

    if (faultFlags == 0) {
        oled.println("No Active Faults");
    } else {
        if (faultFlags & 0x01) oled.println("ERR: Temp High");
        if (faultFlags & 0x02) oled.println("ERR: Temp Low");
        if (faultFlags & 0x04) oled.println("ERR: Volt High");
        if (faultFlags & 0x08) oled.println("ERR: Volt Low");
    }

    oled.print("Events: ");
    oled.println((logFull ? LOG_SIZE : logIndex));
    oled.display();
}

void uploadToCloud() {
    if (WiFi.status() != WL_CONNECTED) return;

    HTTPClient http;
    String url = "https://" FIREBASE_HOST "/faults.json?auth=" FIREBASE_AUTH;
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    String payload = "[";
    int start = logFull ? logIndex : 0;
    int end = logFull ? (logIndex - 1 + LOG_SIZE) % LOG_SIZE : logIndex - 1;

    for (int i = start; i != end; i = (i + 1) % LOG_SIZE) {
        payload += "{\"flags\":" + String(faultLog[i].flags) +
                   ",\"ts\":" + String(faultLog[i].timestamp) + "},";
    }
    if (logIndex != 0 || logFull)
        payload += "{\"flags\":" + String(faultLog[end].flags) +
                   ",\"ts\":" + String(faultLog[end].timestamp) + "}";
    payload += "]";

    int httpCode = http.POST(payload);
    if (httpCode > 0) {
        Serial.println("Uploaded to Firebase!");
    } else {
        Serial.println("Upload failed");
    }
    http.end();
}