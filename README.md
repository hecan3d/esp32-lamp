##💡 ESP32 Smart Web Lamp

A lightweight, self-hosted web-controlled lighting system built for the ESP32 platform. This project turns traditional desk lamps, LED strips, or custom light fixtures into smart home devices using hardware-accelerated PWM dimming and a responsive embedded HTTP control panel.

---

## 🌟 Key Features

* **Embedded Web UI**: Dark-themed, mobile-first responsive web interface stored entirely in memory (no external CDN or filesystem required).
* **Smooth PWM Dimming**: High-frequency Pulse Width Modulation (5 kHz) for flicker-free brightness control and silent hardware operation.
* **RESTful JSON API**: Simple HTTP endpoints for toggling power, adjusting brightness levels, and polling real-time device state.
* **Dual-Channel Ready (CCT)**: Designed with multi-channel support in mind, allowing straightforward expansion to Dual-White (Cold/Warm temperature tuning) or RGB LED setups.
* **Zero Dependencies**: Lightweight C++ codebase relying solely on native ESP32 Arduino core libraries (`WiFi.h` and `WebServer.h`).

---

## 🛠️ Hardware Requirements

* **Microcontroller**: ESP32 DevKit (or any ESP32 variant)
* **Power Control**: N-Channel MOSFET module (e.g., IRLZ44N, AOD4184, or similar logic-level gate MOSFET)
* **Light Source**: 12V / 24V LED strip, LED panel, or converted desk lamp
* **Power Supply**: Suitable DC power adapter matching your LED voltage rating

---

## 🔌 Circuit & Pinout

| ESP32 GPIO Pin | Function | Description |
| :--- | :--- | :--- |
| **GPIO 13** | PWM Output (Cold Light) | Gate driver for Channel 1 MOSFET |
| **GPIO 12** | PWM Output (Warm Light) | Gate driver for Channel 2 MOSFET *(Optional)* |
| **GND** | Common Ground | Connected to power supply ground and MOSFET source |

---

## 📡 API Reference

The onboard web server exposes a RESTful API returning structured JSON responses, making it easy to integrate with custom scripts, iOS Shortcuts, or Home Assistant setups.

### 1. Toggle or Set Power State
* **URL:** `/api/power`
* **Method:** `GET`
* **Parameters:**
  * `state` (`1` or `0`): Turn the light ON or OFF.
  * `toggle` (`1`): Invert the current power state.
* **Example:** `http://<device-ip>/api/power?state=1`
* **Response:**
  ```json
  {
    "status": "ok",
    "power": true
  }

2. Adjust Brightness

    URL: /api/brightness

    Method: GET

    Parameters:

        value (0 to 255): Raw 8-bit PWM duty cycle value.

    Example: http://<device-ip>/api/brightness?value=180

    Response:
    JSON

    {
      "status": "ok",
      "brightness": 180
    }

3. Fetch Device Status

    URL: /api/status

    Method: GET

    Example: http://<device-ip>/api/status

    Response:
    JSON

    {
      "power": true,
      "brightness": 128,
      "pct": 50
    }

🚀 Quick Start & Installation

    Clone or download this repository to your local machine.

    Open the project file (.ino) in the Arduino IDE or PlatformIO.

    Select your ESP32 board model and matching COM port.

    Update the Wi-Fi credentials in the source code:
    C++

    const char* ssid     = "YOUR_WIFI_SSID";
    const char* password = "YOUR_WIFI_PASSWORD";

    Flash the firmware to your ESP32.

    Open the Serial Monitor at 115200 baud rate to find the assigned local IP address.

    Open any browser and enter the device IP address to access the dashboard.

🗺️ Roadmap & Future Enhancements

    [ ] Non-volatile storage (EEPROM / NVS) to persist brightness and state across reboots.

    [ ] AsyncWebServer & WebSocket integration for real-time UI updates across multiple client devices.

    [ ] MQTT support for seamless Home Assistant auto-discovery.

    [ ] Onboard Wi-Fi Manager for initial AP-mode setup without hardcoding network credentials.
