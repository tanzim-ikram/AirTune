# 🎵 AirTune — ESP32 BLE Gesture Media Controller

AirTune is a smart, gesture-based Bluetooth Low Energy (BLE) media controller powered by an ESP32. With the PAJ7620U2 gesture sensor, users can control music playback, adjust volume, and toggle mute using simple hand gestures. It also includes a physical button to launch YouTube and an OLED screen for interactive feedback.

## 🚀 Features

- 🤚 Gesture-based control using the PAJ7620U2 sensor:
  - **Wave / Forward / Backward** → Play/Pause
  - **Right Swipe** → Next Song
  - **Left Swipe** → Previous Song
  - **Clockwise Motion** → Volume Up (+2%)
  - **Anti-Clockwise Motion** → Volume Down (−2%)
  - **Up / Down** → Mute/Unmute

- 🔘 Button functionality:
  - Launches YouTube in browser (`Win + R → https://www.youtube.com`)

- 💡 RGB LED Indicators:
  - 🔴 Red → Not connected to BLE
  - 🟡 Yellow → Connected but idle
  - 🟢 Green → Gesture detected and active

- 📺 OLED Display:
  - Real-time gesture action feedback
  - Idle screen showing "AirTune Ready"

## 🧰 Hardware Components

- ESP32 Dev Board
- DFRobot PAJ7620U2 Gesture Sensor (I2C)
- SSD1306 OLED Display (128x64, I2C)
- RGB LEDs (Red, Green, Blue)
- Push Button (YouTube launcher)
- 220–330Ω resistors (for LEDs)

## 📌 Pin Configuration

| Component       | GPIO | Description                |
|-----------------|------|----------------------------|
| PAJ7620U2       | I2C  | SDA/SCL (default I2C pins) |
| OLED Display    | I2C  | SDA/SCL (0x3C address)     |
| Red LED         | 4    | BLE disconnected status    |
| Green LED       | 16   | Gesture detected           |
| Blue LED        | 17   | BLE connected (idle)       |
| YouTube Button  | 23   | Opens YouTube when pressed |

## 🖥️ How It Works

- Connects to your PC or smartphone as a BLE keyboard.
- Sends standard **media key** codes (e.g., Play/Pause, Volume Up).
- Detects hand gestures in real-time via the PAJ7620U2 sensor.
- Displays context-specific messages on an OLED screen.
- Automatically shows an idle screen after inactivity.

## 🔧 Setup Instructions

1. **Install Arduino IDE** and required libraries:
   - `DFRobot_PAJ7620U2`
   - `BleKeyboard` by T-vK
   - `Adafruit_SSD1306`
   - `Adafruit_GFX`

2. **Connect hardware** to ESP32 as per pin mapping.

3. **Upload Code** using ESP32 board support in Arduino.

4. **Pair ESP32** with your device via Bluetooth as `AirTune`.

## 🧠 Future Ideas

- Add customizable gestures or modes
- Display current song title (via Bluetooth A2DP metadata or integration)

## 📝 License

This project is licensed under the MIT License — free for personal and educational use.
