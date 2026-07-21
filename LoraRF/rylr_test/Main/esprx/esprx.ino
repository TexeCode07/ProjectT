#include <SoftwareSerial.h>

#define RX_PIN 18
#define TX_PIN 19
#define LED_PIN 2
#define BAUD_RATE 115200

SoftwareSerial rylrSerial(RX_PIN, TX_PIN);

void setup() {
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(115200);
    rylrSerial.begin(BAUD_RATE);

    // delay(2000);  // Increased delay

    // Clear buffer
    while (rylrSerial.available()) rylrSerial.read();

    Serial.println("Sending AT command...");
    rylrSerial.print("AT\r\n");
    rylrSerial.flush();
    waitForResponse();

    // Set module parameters
    rylrSerial.print("AT+PARAMETER=10,7,1,7\r\n");
    rylrSerial.flush();
    waitForResponse();

    Serial.println("LoRa RX Ready!");
}

void loop() {
    if (rylrSerial.available()) {
        String receivedMessage = rylrSerial.readStringUntil('\n');
        receivedMessage.trim();
        Serial.println("Received: " + receivedMessage);

        if (receivedMessage.indexOf("A") != -1) {
            digitalWrite(LED_PIN, HIGH);
            delay(1000);
            digitalWrite(LED_PIN, LOW);

            rylrSerial.print("AT+SEND=0,1,Y\r\n");
            rylrSerial.flush();
            waitForResponse();
        }
    }
}

void waitForResponse() {
    long timeout = millis() + 3000;
    while (millis() < timeout) {
        if (rylrSerial.available()) {
            String response = rylrSerial.readStringUntil('\n');
            response.trim();
            Serial.println("RYLR898 Response: " + response);

            if (response.startsWith("+ERR")) {
                Serial.println("Error: Check AT command syntax or parameters.");
            }
            break;
        }
    }
}
