//Receiver :
// select ESP32 Wrover kit ( all versions)
#include <SPI.h>
#include <LoRa.h>

// Define the pins used by the LoRa transceiver module
#define ss   5  // Slave Select pin for LoRa
#define rst  14   // Reset pin for LoRa
#define dio0 2  // Digital IO pin for LoRa interrupt

// Define serial ports for communication
#define SerialMon Serial    // Serial monitor
#define SerialAT Serial1    // Serial communication with GSM modem

// GSM settings
char apn[] = "mnet";    // Access Point Name for GSM
char user[] = "";       // Username for GSM, if needed
char pass[] = "";       // Password for GSM, if needed
#define GSM_PIN ""      // PIN for SIM card, if required
#define TINY_GSM_MODEM_SIM7600  // GSM modem type
#define TINY_GSM_RX_BUFFER 1024  // Set RX buffer size for GSM communication

#include <TinyGsmClient.h>   // Library for GSM communication
#include "utilities.h"      // Utility functions

// Create an instance of the TinyGsmClient for GSM communication
TinyGsm modem(SerialAT);

// Define LED pins
#define red   12   // Red LED pin
#define blue  15   // Blue LED pin
#define green 13   // Green LED pin (Start LED)

// Server and path details for HTTP request
const char server[] = "ernd.tech";                  // Server address
const char path[] = "/test/csvmap.php?doorNumbers="; // Path including query parameters

// GSM client for HTTP
TinyGsmClient client(modem);

unsigned long startTime;
unsigned long duration = 60000;  // 1 minute in milliseconds
String receivedDoors = "";

void setup() {
 // Initialize Serial Monitor for debugging
 Serial.begin(115200);
 while (!Serial);  // Wait until Serial Monitor is ready
 Serial.println("LoRa Receiver");

 // Setup LoRa transceiver module
 LoRa.setPins(ss, rst, dio0);

 // Start AT mode for GSM communication
 SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
 startModem();  // Initialize the GSM modem

 // Start LoRa module on the specified frequency
 while (!LoRa.begin(915E6)) {
   Serial.println(".");
   delay(500);
 }
 LoRa.setSyncWord(0xF3);  // Set LoRa sync word
 Serial.println("LoRa Initializing OK!");

 // Connect to GPRS for GSM communication
 if (!modem.gprsConnect(apn, user, pass)) {
   Serial.println("Failed to connect to GPRS");
   return;
 }

 Serial.println("Connected to GPRS");

 blinkLED(blue);  // Blink the blue LED to indicate connection
 blinkLED(blue);
 blinkLED(blue);

 startTime = millis();  // Initialize start time
}

void loop() {
 // Check for incoming LoRa packets for 1 minute
 while (millis() - startTime < duration) {
   if (LoRa.parsePacket()) {
   String packet = "";
   while (LoRa.available()) {
       packet += (char)LoRa.read();
   }
   // Extract door number from the packet and add it to the list
   int startPos = packet.indexOf(">") + 6;
   int endPos = packet.indexOf("<");
   String doorNum = packet.substring(startPos, endPos);
   receivedDoors += doorNum + ",";
   }
 }

 // Perform HTTP POST request with collected door numbers
 if (receivedDoors.length() > 0) {
   receivedDoors.remove(receivedDoors.length() - 1); // Remove last comma
   postReq(receivedDoors);  // Send the collected data
 }

 // Reset variables for the next iteration
 receivedDoors = "";
 startTime = millis();
}

// Function to initialize the GSM modem
void startModem() {
 // Power on and initialize GSM modem
 pinMode(MODEM_PWRKEY, OUTPUT);
 digitalWrite(MODEM_PWRKEY, HIGH);
 delay(300);
 digitalWrite(MODEM_PWRKEY, LOW);

 pinMode(MODEM_FLIGHT, OUTPUT);
 digitalWrite(MODEM_FLIGHT, HIGH);

 // Attempt to start the modem
 for (int i = 0; i < 3; ++i) {
   while (!modem.testAT(5000)) {
   Serial.println("Try to start modem...");
   pinMode(MODEM_PWRKEY, OUTPUT);
   digitalWrite(MODEM_PWRKEY, HIGH);
   delay(300);
   digitalWrite(MODEM_PWRKEY, LOW);
   }
 }

 // Restart the modem if needed
 SerialMon.println("Initializing modem...");
 if (!modem.restart()) {
   Serial.println("Failed to restart modem, attempting to continue without restarting");
 }
}

// Function to perform an HTTP POST request with collected door numbers
void postReq(String doors) {
 Serial.print("doorNumData:");
 Serial.println(doors);





 // Connect to the server
 if (client.connect(server, 80)) {
   // Send HTTP POST request
   client.println("POST " + String(path) + " HTTP/1.1");
   client.println("Host: " + String(server));
   client.println("Connection: close");
   client.println("Cache-Control: no-cache");
   client.println();

   // Send the collected data
   client.println(doors);

   // Read and print the server response
   while (client.connected()) {
   if (client.available()) {
       char c = client.read();
       Serial.print(c);
   }
   }
   client.stop();  // Close the connection
 } else {
   Serial.println("connection failed");
 }
}

// Function to blink an LED
void blinkLED(int pin) {
 digitalWrite(pin, LOW);
 delay(1500);
 digitalWrite(pin, HIGH);
 delay(1500);
}










