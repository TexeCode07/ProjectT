#include <Adafruit_NeoPixel.h>

#define LED_PIN     8    // The pin where your LED strip is connected
#define TOUCH_PIN   2    // The pin where your touch sensor is connected
#define NUM_LEDS    44   // Number of LEDs in your strip

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

bool ledState = false;        // Track whether LEDs are on or off
bool lastTouchState = false;  // Track the last state of the touch sensor
int brightness = 0;           // Current brightness level (0-255)
int touchMode = 0;            // Track the current mode of the touch sensor (0 to 3)
const int brightnessLevels[] = {0, 102, 178, 255};  // Brightness levels for each mode (0%, 40%, 70%, 100%)

#define DELAYVAL 200          // Time (in milliseconds) to pause between pixels
#define DEBOUNCE_DELAY 50     // Debounce delay in milliseconds

unsigned long lastDebounceTime = 0; // The last time the output pin was toggled

void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.show();  // Initialize all pixels to 'off'
  Serial.println("Setup started...");
  
  pinMode(TOUCH_PIN, INPUT);
}

void loop() {
  bool currentTouchState = digitalRead(TOUCH_PIN);

  // If the current state is different from the last state, reset the debounce timer
  if (currentTouchState != lastTouchState) {
    lastDebounceTime = millis();
  }

  // If the time since the last state change is greater than the debounce delay
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    // If the touch state has changed (stable state after debounce)
    if (currentTouchState != lastTouchState) {
      lastTouchState = currentTouchState;
      // Only toggle LED state on the rising edge of the touch input
      if (currentTouchState == HIGH) {
        touchMode = (touchMode + 1) % 4;  // Cycle through modes 0 to 3
        brightness = brightnessLevels[touchMode];
        ledState = brightness > 0;
        
        // Debugging output
        Serial.print("Touch detected! Mode: ");
        Serial.print(touchMode);
        Serial.print(", Brightness: ");
        Serial.println(brightness);
        updateLEDs();
      }
    }
  }

  delay(1);  // Small delay for responsiveness
}

void updateLEDs() {
  if (ledState) {
    strip.clear();
    // Turn on LEDs to warm white with current brightness
    for(int i = 0; i < NUM_LEDS; i++) {
      uint8_t r = 255 * brightness / 255;
      uint8_t g = 147 * brightness / 255;
      uint8_t b = 41 * brightness / 255;
      strip.setPixelColor(i, strip.Color(r, g, b));
      delay(DELAYVAL);
    }
  } else {
    // Turn off all LEDs
    strip.clear();
  }
  strip.show();  // Update the strip
}