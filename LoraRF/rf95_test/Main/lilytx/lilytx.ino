#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 5
#define RFM95_RST 14
#define RFM95_INT 2
#define LED_PIN 12
#define BOOT_BUTTON_PIN 13  // GPIO 0 is the boot button

RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {
    pinMode(LED_PIN, OUTPUT);
    pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP); // Configure boot button with pull-up
    Serial.begin(9600);

    if (!rf95.init()) {
        Serial.println("LoRa init failed");
        while (1);
    }
    Serial.println("LoRa TX Ready!");
    rf95.setFrequency(915.0);
}

void loop() {
    // Check if the boot button is pressed (LOW indicates pressed)
    if (digitalRead(BOOT_BUTTON_PIN) == LOW) {
        // Send "A"
        const char* msg = "A";
        rf95.send((uint8_t*)msg, strlen(msg));
        rf95.waitPacketSent();

        // Blink LED to indicate transmission
        digitalWrite(LED_PIN, HIGH);
        delay(100);
        digitalWrite(LED_PIN, LOW);

        // Wait to receive "Y" as feedback from RX
        if (rf95.waitAvailableTimeout(2000)) {
            uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
            uint8_t len = sizeof(buf);

            if (rf95.recv(buf, &len)) {
                // Check if feedback received is "Y"
                if (len == 1 && buf[0] == 'Y') {
                    digitalWrite(LED_PIN, HIGH);
                    delay(1000); // Blink LED for 1 second
                    digitalWrite(LED_PIN, LOW);
                }
            }
        }

        delay(2000); // Delay to debounce button press
    }
}
