#include <SoftwareSerial.h>

#define RX_PIN 18         // Change this to your chosen RX pin (ESP32 RX GPIO)
#define TX_PIN 19         // Change this to your chosen TX pin (ESP32 TX GPIO)
#define BAUD_RATE 115200  // RYLR898 default baud rate

SoftwareSerial rylrSerial(RX_PIN, TX_PIN);

void setup() {
    Serial.begin(115200);
    rylrSerial.begin(BAUD_RATE);

    Serial.println("RYLR898 AT Command Tester");
    Serial.println("Type AT commands and press Enter.");
    Serial.println("------------------------------------------------");
}

void loop() {
    // If there's input from the Serial Monitor, send it to RYLR898
    if (Serial.available()) {
        String atCommand = Serial.readStringUntil('\n');
        rylrSerial.println(atCommand);  // Send the AT command to RYLR898
        Serial.println("Sent: " + atCommand);
    }

    // If there's a response from the RYLR898, print it to the Serial Monitor
    if (rylrSerial.available()) {
        String response = rylrSerial.readStringUntil('\n');
        Serial.println("Response: " + response);
    }
}
