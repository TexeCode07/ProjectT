#include <PZEM004Tv30.h>

#define PZEM_SERIAL Serial2
#define PZEM_RX_PIN 16
#define PZEM_TX_PIN 17

// RX pin, TX pin (connect PZEM TX to ESP32 RX, PZEM RX to ESP32 TX)
PZEM004Tv30 pzem(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN);

// Relay Pin
const int relayPin = 4;

// Current threshold (Amps)
const float currentThreshold = 10.0; // Set this to your desired threshold

void setup() {
  Serial.begin(115200);
  
  // Initialize relay pin as an output
  pinMode(relayPin, OUTPUT);
  // Start with the relay off
  digitalWrite(relayPin, LOW);

}


void loop() {
  // put your main code here, to run repeatedly:
   // Read the data from the sensor
    float voltage = pzem.voltage();
    float current = pzem.current();
    float power = pzem.power();
    float energy = pzem.energy();
    float frequency = pzem.frequency();
    float pf = pzem.pf();

    // Check if the data is valid
    if(isnan(voltage)){
        Serial.println("Error reading voltage");
    } else if (isnan(current)) {
        Serial.println("Error reading current");
    } else if (isnan(power)) {
        Serial.println("Error reading power");
    } else if (isnan(energy)) {
        Serial.println("Error reading energy");
    } else if (isnan(frequency)) {
        Serial.println("Error reading frequency");
    } else if (isnan(pf)) {
        Serial.println("Error reading power factor");
    } else {

      // Print the values to the Serial console
      Serial.print("Voltage: ");      Serial.print(voltage);      Serial.println("V");
      Serial.print("Current: ");      Serial.print(current);      Serial.println("A");
      Serial.print("Power: ");        Serial.print(power);        Serial.println("W");
      Serial.print("Energy: ");       Serial.print(energy,3);     Serial.println("kWh");
      Serial.print("Frequency: ");    Serial.print(frequency, 1); Serial.println("Hz");
      Serial.print("PF: ");           Serial.println(pf);


      if (current > currentThreshold) {
        // Current is above threshold, turn the relay on
        digitalWrite(relayPin, HIGH);
        Serial.println("Relay ON");
      } else {
        // Current is below threshold, turn the relay off
        digitalWrite(relayPin, LOW);
        Serial.println("Relay OFF");
      }



    }
}
