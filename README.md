# ESP8266/ESP32 WiFi & Telegram Bot

## Overview
This project uses an ESP8266 or ESP32 to connect to a WiFi network, store credentials in SPIFFS, and send a Telegram message when triggered. The device supports a reset button to clear stored WiFi credentials and enter configuration mode.

## Features
- Connects to WiFi using stored credentials or WiFiManager
- Saves and loads WiFi credentials from SPIFFS
- Sends a Telegram message when triggered
- Supports deep sleep mode for power efficiency
- Reset button to clear WiFi settings

## Hardware Requirements
- ESP8266 (e.g., NodeMCU) or ESP32
- Push button for reset functionality
- Internet connection

## Software Requirements
- Arduino IDE with ESP8266 or ESP32 board support
- Required Libraries:
  - `ESP8266WiFi` / `WiFi.h`
  - `ESP8266WebServer` / `WebServer.h`
  - `FS.h`
  - `ArduinoJson`
  - `WiFiClientSecure`
  - `UniversalTelegramBot`
  - `WiFiManager`

## Installation
1. Clone this repository:
   ```sh
   git clone https://github.com/Salumi2023/rs-door.git
   ```
2. Open the project in Arduino IDE.
3. Install the required libraries via Library Manager.
4. Compile and upload the code to your ESP8266/ESP32.

## Usage
1. On first boot, the device starts in WiFiManager mode.
2. Connect to the `ESP_Config` WiFi network and enter your WiFi credentials.
3. Once connected, credentials are stored, and the device will reconnect automatically.
4. If the reset button is held for more than 3 seconds, stored WiFi credentials are erased.
5. When triggered, the device sends a Telegram message.
6. After execution, the ESP goes into deep sleep mode.

## Configuration
- Modify `BOT_TOKEN` and `CHAT_ID` in the code to match your Telegram bot settings.
- Set the correct reset pin based on your ESP model:
  ```cpp
  #define RESET_PIN 2 // ESP-01
  #define RESET_PIN 5 // ESP32 or ESP12 (NodeMCU)
  ```

## Troubleshooting
- If the device fails to connect to WiFi:
  - Check stored credentials by viewing `/wifi.json` in SPIFFS.
  - Hold the reset button for 3+ seconds to clear settings.
- If the Telegram message is not sent:
  - Ensure `BOT_TOKEN` and `CHAT_ID` are correctly set.
  - Verify that the ESP has internet access.

## License
This project is licensed under the MIT License.

## Author
Salumi2023

