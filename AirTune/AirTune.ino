#include <DFRobot_PAJ7620U2.h>
#include <BleKeyboard.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Pin definitions
#define LED_R 4
#define LED_G 16
#define LED_B 17
#define BTN_YOUTUBE 23

// OLED display setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

DFRobot_PAJ7620U2 paj;
BleKeyboard bleKeyboard("AirTune", "ESP32", 100);

// Timers
unsigned long lastGestureTime = 0;
unsigned long debounceInterval = 200;
unsigned long lastDisplayUpdate = 0;
unsigned long idleTimeout = 2000;

int volume = 30;
bool sensorTriggered = false;

// ========== Helper Functions ==========

void setRGBColor(bool r, bool g, bool b) {
  digitalWrite(LED_R, r ? HIGH : LOW);
  digitalWrite(LED_G, g ? HIGH : LOW);
  digitalWrite(LED_B, b ? HIGH : LOW);
}

void displayCenteredText(const String &text) {
  display.clearDisplay();
  int16_t x1, y1;
  uint16_t w, h;
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  int x = (SCREEN_WIDTH - w) / 2;
  int y = (SCREEN_HEIGHT - h) / 2;
  display.setCursor(x, y);
  display.println(text);
  display.display();
}

void showIdleScreen() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor((SCREEN_WIDTH - 16) / 2, 8);
  display.write(14);  // Musical note
  display.setTextSize(1);
  display.setCursor((SCREEN_WIDTH - 80) / 2, 36);
  display.println("AirTune Ready");
  display.display();
}

// ========== Setup ==========

void setup() {
  Serial.begin(115200);
  delay(300);

  // Pin setup
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(BTN_YOUTUBE, INPUT_PULLUP);

  setRGBColor(true, false, false); // Initial state: Red (not connected)

  // OLED init
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found!");
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("AirTune Initializing...");
  display.display();

  // Gesture sensor init
  if (paj.begin() != 0) {
    Serial.println("Gesture Sensor Initialization Failed!");
    display.println("Gesture sensor FAIL!");
  } else {
    Serial.println("Gesture Sensor Initialized!");
    display.println("Gesture sensor OK!");
    paj.setGestureHighRate(false);
  }

  bleKeyboard.begin();
  display.println("BLE OK!");
  display.display();

  delay(1000);
  showIdleScreen();
}

// ========== Loop ==========

void loop() {
  bool isConnected = bleKeyboard.isConnected();

  // RGB LED logic
  if (!isConnected) {
    setRGBColor(true, false, false); // Red
  } else if (sensorTriggered) {
    setRGBColor(false, true, false); // Green (gesture active)
    sensorTriggered = false;        // Reset for next loop
  } else {
    setRGBColor(true, true, false); // Yellow (connected, no gesture)
  }

  // Button handler with debounce
  static bool buttonHandled = false;
  if (isConnected && digitalRead(BTN_YOUTUBE) == LOW && !buttonHandled) {
    Serial.println("Launching YouTube (button)");
    displayCenteredText("Launching YouTube...");
    bleKeyboard.press(KEY_LEFT_GUI);
    bleKeyboard.press('r');
    delay(100);
    bleKeyboard.releaseAll();
    delay(400);
    bleKeyboard.print("https://www.youtube.com");
    delay(100);
    bleKeyboard.write(KEY_RETURN);
    buttonHandled = true;
  }

  if (digitalRead(BTN_YOUTUBE) == HIGH) {
    buttonHandled = false;
  }

  // Gesture handling with fast debounce
  DFRobot_PAJ7620U2::eGesture_t gesture = paj.getGesture();
  unsigned long now = millis();
  if (gesture != paj.eGestureNone && isConnected && (now - lastGestureTime > debounceInterval)) {
    lastGestureTime = now;
    lastDisplayUpdate = now;
    sensorTriggered = true; // Signal to light green LED

    switch (gesture) {
      case paj.eGestureForward:
      case paj.eGestureBackward:
      case paj.eGestureWave:
      case paj.eGestureWaveSlowlyDisorder:
        Serial.println("Play/Pause");
        displayCenteredText("Toggling Play/Pause");
        bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
        break;

      case paj.eGestureRight:
        Serial.println("Next Song");
        displayCenteredText("Playing Next Song");
        bleKeyboard.write(KEY_MEDIA_NEXT_TRACK);
        break;

      case paj.eGestureLeft:
        Serial.println("Previous Song");
        displayCenteredText("Playing Previous Song");
        bleKeyboard.write(KEY_MEDIA_PREVIOUS_TRACK);
        break;

      case paj.eGestureClockwise:
        Serial.println("Volume Up");
        volume = min(100, volume + 2);
        displayCenteredText("Volume: " + String(volume) + "%");
        bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
        break;

      case paj.eGestureAntiClockwise:
        Serial.println("Volume Down");
        volume = max(0, volume - 2);
        displayCenteredText("Volume: " + String(volume) + "%");
        bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
        break;

      case paj.eGestureUp:
      case paj.eGestureDown:
        Serial.println("Mute/Unmute");
        displayCenteredText("Mute/Unmute");
        bleKeyboard.write(KEY_MEDIA_MUTE);
        break;

      default:
        break;
    }
  }

  // Idle screen if no interaction
  if (now - lastDisplayUpdate > idleTimeout) {
    showIdleScreen();
    lastDisplayUpdate = now;
  }
}
