#include <HardwareSerial.h>

#define RX_PIN 18           // RX pin
#define TX_PIN 19           // TX pin
#define LED_PIN 12          // LED pin
#define BOOT_BUTTON_PIN 13  // Boot button pin (Optional)
#define BAUD_RATE 115200    // RYLR898 default baud rate

HardwareSerial rylrSerial(1);  // Using UART1 for communication

void setup() {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.begin(115200);           // For debugging
    rylrSerial.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);

    delay(2000);  // Allow module to initialize

    // Clear buffer
    while (rylrSerial.available()) rylrSerial.read();

    Serial.println("Initializing Receiver...");

    // Initialize the RYLR898 module
    sendATCommand("AT\r\n");
    sendATCommand("AT+ADDRESS=1\r\n");   // Set unique address for receiver
    sendATCommand("AT+NETWORKID=5\r\n"); // Set network ID to match sender
    sendATCommand("AT+BAND=868000000\r\n"); // Set frequency band
    sendATCommand("AT+PARAMETER=10,7,1,7\r\n"); // Set LoRa parameters

    Serial.println("LoRa Receiver Ready!");
}

void loop() {
    if (rylrSerial.available()) {
        String receivedMessage = rylrSerial.readStringUntil('\n');
        receivedMessage.trim();
        Serial.println("Received: " + receivedMessage);

        // Blink LED on receiving a message
        digitalWrite(LED_PIN, HIGH);
        delay(500);
        digitalWrite(LED_PIN, LOW);
        delay(500);
    }
}

// Function to send AT command and wait for response
void sendATCommand(const String &command) {
    rylrSerial.print(command);
    rylrSerial.flush();
    waitForResponse();
}

// Function to wait for response from the LoRa module
void waitForResponse() {
    long timeout = millis() + 3000; // 3 seconds timeout
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
