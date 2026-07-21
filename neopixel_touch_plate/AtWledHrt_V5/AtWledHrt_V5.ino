#include <Adafruit_NeoPixel.h>

// === IP5306 HEARTBEAT CONFIG ===
#define IP5306_HEARTBEAT_PIN 3  // PB3 (physical pin 2)
unsigned long Delay_IP5306_Heartbeat;
unsigned long Delay_IP5306_Heartbeat_Led;
const unsigned long IP5306_HEARTBEAT_TIMEOUT = 30000; // 15 seconds
const unsigned long HOLD_TIME = 150; // 150 ms "button press"
bool ButtonD = false;

// === TOUCH + LED CONFIG ===
#define LED_PIN     1    // PB0 (physical pin 5)
#define TOUCH_PIN   2    // PB2 (physical pin 7)
#define NUM_LEDS    10

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
bool lastTouchState = false;
int touchMode = 0;

void setup() {
  // Serial (optional: only works if USB or UART is connected)
  // Serial.begin(115200);

  // === IP5306 SETUP ===
  pinMode(IP5306_HEARTBEAT_PIN, OUTPUT);
  digitalWrite(IP5306_HEARTBEAT_PIN, LOW);
  Delay_IP5306_Heartbeat_Led = Delay_IP5306_Heartbeat = millis();

  // === TOUCH + LED SETUP ===
  strip.begin();
  strip.clear();
  strip.show();

  pinMode(TOUCH_PIN, INPUT);
}

void loop() {
  // === Handle Touch + LED Mode ===
  bool currentTouchState = digitalRead(TOUCH_PIN);

  if (currentTouchState && !lastTouchState) {
    touchMode = (touchMode + 1) % 5;
    updateLEDs();
  }

  lastTouchState = currentTouchState;

  // === Handle IP5306 Heartbeat Pulse ===
  IP5306_Heartbeat();

  delay(10); // Small delay for responsiveness
}

void IP5306_Heartbeat() {
  if (!ButtonD && (millis() - Delay_IP5306_Heartbeat) >= IP5306_HEARTBEAT_TIMEOUT) {
    ButtonD = true;
    Delay_IP5306_Heartbeat_Led = IP5306_HEARTBEAT_TIMEOUT + HOLD_TIME;
    digitalWrite(IP5306_HEARTBEAT_PIN, HIGH); // "press" button
  }
  else if (ButtonD && (millis() - Delay_IP5306_Heartbeat) >= Delay_IP5306_Heartbeat_Led) {
    ButtonD = false;
    Delay_IP5306_Heartbeat += IP5306_HEARTBEAT_TIMEOUT;
    digitalWrite(IP5306_HEARTBEAT_PIN, LOW); // "release" button
  }
}

void updateLEDs() {
  strip.clear();

  switch (touchMode) {
    case 0: setColor(255, 0, 0); break;   // Red
    case 1: setColor(0, 255, 0); break;   // Green
    case 2: setColor(0, 0, 255); break;   // Blue
    case 3: setColor(255, 255, 255); break; // White
    case 4: /* Off */ break;
  }

  strip.show();
}

void setColor(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
}
