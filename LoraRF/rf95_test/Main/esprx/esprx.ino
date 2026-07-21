#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 5
#define RFM95_RST 14
#define RFM95_INT 4
#define LED_PIN 2

RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(9600);

    if (!rf95.init()) {
        Serial.println("LoRa init failed");
        while (1);
    }
    Serial.println("LoRa RX Ready!");
    rf95.setFrequency(915.0);
}

void loop() {
    // Wait for incoming message
    if (rf95.waitAvailableTimeout(3000)) {
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);

        if (rf95.recv(buf, &len)) {
            // Check if received message is "A"
            if (len == 1 && buf[0] == 'A') {
                // Blink LED for 1 second
                digitalWrite(LED_PIN, HIGH);
                delay(1000);
                digitalWrite(LED_PIN, LOW);

                // Send feedback "Y"
                const char* feedback = "Y";
                rf95.send((uint8_t*)feedback, strlen(feedback));
                rf95.waitPacketSent();
            }
        }
    }
}
