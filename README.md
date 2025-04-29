# 📦 Automated Inventory Tracking Pad

This project is an **IoT-based smart inventory tracking system** built with an ESP32 and HX711 load cell amplifier. It tracks real-time weight changes of items (like poker chips or product packages), logs the data to **Google Sheets**, and sends **email alerts** when items are added or removed. Ideal for inventory monitoring, supply chain automation, or smart shelf systems.

## 🛠️ Features

- 📊 **Real-time Weight Monitoring** via HX711 load cell
- ☁️ **Google Sheets Integration** for cloud-based data logging
- 📧 **Email Notifications** for stock changes (via Gmail SMTP)
- 🔌 **Power-efficient ESP32 microcontroller**
- 📉 **Threshold Detection** to ignore noise or minor fluctuations

## 📷 Hardware Setup

- **ESP32 Development Board** (e.g., ESP-WROOM-32)
- **HX711 Load Cell Amplifier**
- **Load Cell / Strain Gauge Sensor**
- Optional: **OLED display** for local readout (not required)

### Wiring Overview

| HX711 Pin | ESP32 Pin |
|-----------|-----------|
| VCC       | 3.3V      |
| GND       | GND       |
| DT        | GPIO 21   |
| SCK       | GPIO 22   |

> ⚠️ Always use **3.3V** logic with the HX711 when using the ESP32.

## 💻 Software Components

- Arduino IDE with ESP32 board support
- `HX711.h` library for reading the load cell
- `WiFi.h` for network connection
- `HTTPSRedirect` for Google Sheets communication
- `SMTPClient` or `ESP-Mail-Client` for Gmail alerts

## 📈 Google Sheets Integration

1. Create a Google Sheet with the desired format.
2. Use a [Google Apps Script Web App](https://script.google.com) to receive `POST` requests.
3. Deploy the script and copy the URL.
4. Add this URL to your ESP32 code to log data directly.

## 📧 Email Alert Setup

1. Use a dedicated Gmail account.
2. Enable **"Less secure apps"** or generate an **App Password** if using 2FA.
3. Configure SMTP settings in the ESP32 code.

```cpp
smtpClient.setServer("smtp.gmail.com", 465);
smtpClient.setLogin("your-email@gmail.com", "your-app-password");
