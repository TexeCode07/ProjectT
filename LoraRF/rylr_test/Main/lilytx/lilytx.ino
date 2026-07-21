#include <SoftwareSerial.h>

#define RX_PIN 18           // Change this to your chosen RX pin (ESP32 RX GPIO)
#define TX_PIN 19           // Change this to your chosen TX pin (ESP32 TX GPIO)
#define LED_PIN 12          // LED pin
#define BOOT_BUTTON_PIN 13  // Boot button pin (GPIO 0 usually used as BOOT button on ESP32)
#define BAUD_RATE 115200    // RYLR898 default baud rate

SoftwareSerial rylrSerial(RX_PIN, TX_PIN);

void setup() {
    pinMode(LED_PIN, OUTPUT);
    pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP); // Configure boot button with pull-up
    Serial.begin(115200);
    rylrSerial.begin(BAUD_RATE);

    //delay(2000); // Increased delay for module initialization
    Serial.println("Initializing RYLR898...");

    // Clear serial buffer
    while (rylrSerial.available()) rylrSerial.read();

    // Check communication
    rylrSerial.print("AT\r\n");
    delay(500); // Ensure time for processing
    waitForResponse();

    // Set LoRa parameters (Frequency: 915 MHz)
    rylrSerial.print("AT+PARAMETER=10,7,1,7\r\n"); // SF7, BW 125 kHz, CR 4/5, CRC on
    delay(500);
    waitForResponse();

    Serial.println("LoRa TX Ready!");
}

void loop() {
    // Check if the boot button is pressed (LOW indicates pressed)
    if (digitalRead(BOOT_BUTTON_PIN) == LOW) {
        Serial.println("Button pressed. Sending 'A'...");

        // Send "A" message
        String msg = "AT+SEND=0,1,A\r\n";  // "0" is the destination address, "1" is the length of the message
        rylrSerial.print(msg);
        // delay(500); // Wait for command to process
        waitForResponse();

        // Blink LED to indicate transmission
        digitalWrite(LED_PIN, HIGH);
        delay(100);
        digitalWrite(LED_PIN, LOW);

        // Wait for feedback "Y" from RX for 2 seconds
        long timeout = millis() + 2000;
        while (millis() < timeout) {
            if (rylrSerial.available()) {
                String receivedMessage = rylrSerial.readStringUntil('\n');
                receivedMessage.trim();
                Serial.println("Received: " + receivedMessage);

                // Check if feedback received is "Y"
                if (receivedMessage.indexOf("Y") != -1) {
                    Serial.println("Feedback 'Y' received.");
                    digitalWrite(LED_PIN, HIGH);
                    delay(1000); // Blink LED for 1 second
                    digitalWrite(LED_PIN, LOW);
                }
                break;
            }
        }

        delay(2000); // Debounce delay
    }
}

// Helper function to wait for response from RYLR898
void waitForResponse() {
    long timeout = millis() + 3000;
    while (millis() < timeout) {
        if (rylrSerial.available()) {
            String response = rylrSerial.readStringUntil('\n');
            response.trim();
            Serial.println("RYLR898 Response: " + response);

            // Error handling
            if (response.startsWith("+ERR")) {
                Serial.println("Error: Invalid AT command or parameter.");
            }

            if (response.startsWith("+OK")) {
                Serial.println("Command executed successfully.");
            }
        }
    }
}
