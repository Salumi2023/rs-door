#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <WiFiManager.h>

#ifdef ESP8266
#define RESET_PIN 2   // ESP-01 
#endif

#ifdef ESP32
#define RESET_PIN 5
#endif

#define RESET_PIN 5  // ESP12 (NodeMCU 0.9) = ESP8266 but with more pins

unsigned long buttonPressTime = 0;  // Time the button has been pressed
bool buttonPressed = false;  // Flag to detect whether the button is pressed

#define BOT_TOKEN "7152106777:"
#define CHAT_ID "724441308"

#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);
WiFiManager wifiManager;
ESP8266WebServer server(80);

const char* wifiConfigFile = "/wifi.json";  // File to store WiFi credentials

void saveWiFiCredentials(const char* ssid, const char* password) {
    StaticJsonDocument<256> doc;
    doc["ssid"] = ssid;
    doc["password"] = password;

    File file = SPIFFS.open(wifiConfigFile, "w");
    if (file) {
        serializeJson(doc, file);
        file.close();
        Serial.println("WiFi credentials saved!");
    } else {
        Serial.println("Error saving WiFi credentials.");
    }
}

bool loadWiFiCredentials(String& ssid, String& password) {
    File file = SPIFFS.open(wifiConfigFile, "r");
    if (!file) {
        Serial.println("No saved WiFi credentials found.");
        return false;
    }

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.println("Error loading WiFi credentials.");
        return false;
    }

    ssid = doc["ssid"].as<String>();
    password = doc["password"].as<String>();

    return ssid.length() > 0;
}

void setup() {
    Serial.begin(9600);
    pinMode(RESET_PIN, INPUT_PULLUP);  // Set the reset pin as input with pull-up resistor

    if (!SPIFFS.begin()) {
        Serial.println("SPIFFS could not be mounted.");
    }

    // Check if the reset button is pressed
    while (digitalRead(RESET_PIN) == LOW) {  // While the button is pressed
        if (!buttonPressed) {
            buttonPressTime = millis();  // Record the time of the first press
            buttonPressed = true;
            Serial.println("Button pressed...");
        }

        // If the button is pressed for more than 3 seconds
        if (millis() - buttonPressTime > 3000) {
            Serial.println("Resetting WiFi settings...");
            wifiManager.resetSettings();
            SPIFFS.remove(wifiConfigFile);  // Delete the saved WiFi credentials
            ESP.restart();  // Restart the ESP
        }
    }

    // If the button is released, check if the time was not exceeded
    if (buttonPressed && (millis() - buttonPressTime < 3000)) {
        Serial.println("Button was pressed but too short for reset.");
    }

    Serial.println("Loading saved WiFi credentials...");
    String ssid, password;
    
    if (loadWiFiCredentials(ssid, password)) {
        Serial.println("Saved SSID: " + ssid);
        WiFi.begin(ssid.c_str(), password.c_str());
    } else {
        Serial.println("No saved WiFi credentials found. Starting WiFiManager...");

        wifiManager.setBreakAfterConfig(true);  
        if (!wifiManager.autoConnect("ESP_Config")) {
            Serial.println("WiFiManager failed, restarting ESP...");
            ESP.restart();
        }

        ssid = WiFi.SSID();
        password = WiFi.psk();
        saveWiFiCredentials(ssid.c_str(), password.c_str());

        WiFi.begin();
    }

    int timeout = 20;
    while (WiFi.status() != WL_CONNECTED && timeout > 0) {
        delay(500);
        Serial.print(".");
        timeout--;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected!");
    } else {
        Serial.println("\nWiFi connection failed.");
    }

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    configTime(0, 0, "pool.ntp.org");

    #ifdef ESP8266
        client.setTrustAnchors(&cert);
        Serial.println("Certificate set for ESP8266.");
    #endif

    #ifdef ESP32
        client.setTrustAnchors(&cert);
        Serial.println("Certificate set for ESP32.");
    #endif

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Sending Telegram message...");
        if (bot.sendMessage(CHAT_ID, "DOOR opened", "")) {
            Serial.println("Telegram message sent successfully.");
        } else {
            Serial.println("Failed to send Telegram message.");
        }
    } else {
        Serial.println("WiFi connection failed. Cannot send message.");
    }

    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    delay(100);
    Serial.println("Going to deep sleep...");
    ESP.deepSleep(0);
}

void loop() {
}
