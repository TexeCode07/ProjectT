#include <Adafruit_NeoPixel.h>

#define LED_PIN     1    // The pin where your LED strip is connected
#define TOUCH_PIN   2    // The pin where your touch sensor is connected
#define NUM_LEDS    35   // Number of LEDs in your strip

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

bool lastTouchState = false;  // Track the last state of the touch sensor
int touchMode = 0;            // Track the current mode of the touch sensor (0 to 4)

void setup() {
  Serial.begin(115200);
  Serial.println("Setup started...");
  
  strip.begin();
  // strip.show();  // Initialize all pixels to 'off'
  
  pinMode(TOUCH_PIN, INPUT);
}

void loop() {
  bool currentTouchState = digitalRead(TOUCH_PIN);

  // Check for touch sensor change (rising edge)
  if (currentTouchState && !lastTouchState) {
    touchMode = (touchMode + 1) % 5;  // Cycle through modes 0 to 4
    Serial.print("Touch detected! Mode: ");
    Serial.println(touchMode);
    updateLEDs();
  }

  lastTouchState = currentTouchState;

  delay(10);  // Small delay for responsiveness
}

void updateLEDs() {
  strip.clear();  // Clear previous LED settings

  switch (touchMode) {
    case 0:
      // Mode 1: Solid Red
      Serial.println("Mode: Solid Red");
      setColor(255, 0, 0);
      break;

    case 1:
      // Mode 2: Solid Green
      Serial.println("Mode: Solid Green");
      setColor(0, 255, 0);
      break;

    case 2:
      // Mode 3: Solid Blue
      Serial.println("Mode: Solid Blue");
      setColor(0, 0, 255);
      break;

    case 3:
      // Mode 4: Solid White
      Serial.println("Mode: Solid White");
      setColor(255, 255, 255);
      break;
    
    case 4:
      // Mode 0: LEDs Off
      Serial.println("Mode: Off");
      break;

  }

  strip.show();  // Update the strip
}

// Helper function to set all LEDs to a single color
void setColor(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
}
