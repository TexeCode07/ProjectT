#include <SPI.h>
#include <RH_RF95.h>

// Define pins for RF95 and LED
#define RFM95_CS 5
#define RFM95_RST 14
#define RFM95_INT 2
#define LED_PIN 12

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
    Serial.println("LoRa RX Ready!");

    // Optional: Set frequency to 915MHz
    rf95.setFrequency(915.0);
}

void loop() {
    if (rf95.available()) {
        // Buffer to hold received message
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);

        // Check if received message is valid
        if (rf95.recv(buf, &len)) {
            Serial.print("Received: ");
            Serial.println((char*)buf);

            // Blink LED to indicate successful reception
            digitalWrite(LED_PIN, HIGH);
            delay(1000);
            digitalWrite(LED_PIN, LOW);
            delay(1000);
        }
    }
}
