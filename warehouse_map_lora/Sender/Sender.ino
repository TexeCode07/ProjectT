//Sender.ino   Arduino Code :
// device select  ESP32 wrover kit(all versions) / port find avalibel .
#include <SPI.h>
#include <LoRa.h>

// Define the pins used by the LoRa transceiver module
#define ss   5   // Slave Select (SS) pin
#define rst 14   // Reset (RST) pin
#define dio0 2   // Digital Input/Output 0 (DIO0) pin
int counter = 0;

void setup() {
 // Initialize Serial Monitor for debugging
 Serial.begin(115200);
 while (!Serial); // Wait for Serial Monitor to open
 Serial.println("LoRa Sender");

 // Setup LoRa transceiver module using specified pins for SS, RST, and DIO0
 LoRa.setPins(ss, rst, dio0);
 // Initialize the LoRa module with a specified frequency (915 MHz in this case)
 // Keep trying until LoRa.begin() returns true, indicating successful initialization
 while (!LoRa.begin(915E6)) {
   Serial.println("."); // Display dots while waiting for initialization
   delay(500);
 }
 // Change the sync word to 0xF3 to match the receiver's sync word
 // This ensures that only LoRa messages with the same sync word are received
 LoRa.setSyncWord(0xF3);
 Serial.println("LoRa Initializing OK!");
}

void loop() {
 // Display the packet number being sent in the Serial Monitor
 Serial.print("Sending packet: ");
 Serial.println(counter);

 // Send a LoRa packet to the receiver
 LoRa.beginPacket();
 
 // Transmit a message ('>DOOR_3<') to the receiver , upload it as it defined as door3 , change this parameter for other device name >DOOR_xxx<
 LoRa.print(">DOOR_3<"); 
 
 LoRa.endPacket();

 // Increment the counter for the next packet
 counter++;

 // Wait for a delay of 10 seconds before sending the next packet
 delay(10000);
}





