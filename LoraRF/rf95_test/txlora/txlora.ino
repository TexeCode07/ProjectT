#include <SPI.h>
#include <RH_RF95.h>

// Define pins for RF95 and LED
#define RFM95_CS 5
#define RFM95_RST 14
#define RFM95_INT 4
#define LED_PIN 2

// Initialize RF95
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(9600);

    // LoRa module setup
    if (!rf95.init()) {
        Serial.println("LoRa init failed");
        while (1);
    }
    Serial.println("LoRa TX Ready!");

    // Optional: Set frequency to 915MHz
    rf95.setFrequency(915.0);
}

void loop() {
    // Message to send
    const char* msg = "Hello LoRa RX";
    
    // Send message
    rf95.send((uint8_t*)msg, strlen(msg));
    rf95.waitPacketSent();
    
    // Blink LED to indicate transmission
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_PIN, LOW);

    // Wait for a second
    delay(2000);
}
