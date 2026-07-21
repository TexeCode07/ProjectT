/*
 * IP5306 Heartbeat sketch adapted for ATtiny85
 *
 * Simulates button press every 15 seconds to keep IP5306 awake
 *
 * @author     Adapted by ChatGPT
 * @version    V1.2.1
 * @since      25th April 2025
 * @hardware   ATtiny85
 */

// Pin definitions
#define IP5306_HEARTBEAT_PIN 3  // ATtiny85 pin PB3 (digital pin 3)

unsigned long Delay_IP5306_Heartbeat;
unsigned long Delay_IP5306_Heartbeat_Led;

const unsigned long IP5306_HEARTBEAT_TIMEOUT = 30000; // 15 seconds between heartbeats
const unsigned long HOLD_TIME = 100; // Button "hold" time in ms

bool ButtonD = false;

void setup() {
  pinMode(IP5306_HEARTBEAT_PIN, OUTPUT);
  digitalWrite(IP5306_HEARTBEAT_PIN, HIGH); // Ensure pin is low initially

  Delay_IP5306_Heartbeat_Led = Delay_IP5306_Heartbeat = millis(); // Initialize timers
}

void IP5306_Heartbeat() {
  /*
   * Simulates a button press using BC547B every 15s.
   * IP5306 interprets 30ms–2s press as a valid "short press"
   */
  unsigned long now = millis();

  if (!ButtonD && (now - Delay_IP5306_Heartbeat) >= IP5306_HEARTBEAT_TIMEOUT) {
    ButtonD = true;
    Delay_IP5306_Heartbeat_Led = IP5306_HEARTBEAT_TIMEOUT + HOLD_TIME;
    digitalWrite(IP5306_HEARTBEAT_PIN, HIGH); // Press
  }
  else if (ButtonD && (now - Delay_IP5306_Heartbeat) >= Delay_IP5306_Heartbeat_Led) {
    ButtonD = false;
    Delay_IP5306_Heartbeat += IP5306_HEARTBEAT_TIMEOUT;
    digitalWrite(IP5306_HEARTBEAT_PIN, LOW); // Release
  }
}

void loop() {
  IP5306_Heartbeat(); // Run heartbeat logic
}
