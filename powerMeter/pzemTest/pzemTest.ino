#include <PZEM004Tv30.h>

#define PZEM_SERIAL1 Serial2 // Use Serial2 for the first PZEM
#define PZEM_RX_PIN1 16
#define PZEM_TX_PIN1 17
PZEM004Tv30 pzem1(PZEM_SERIAL1, PZEM_RX_PIN1, PZEM_TX_PIN1);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);     // For debug messages
  PZEM_SERIAL1.begin(9600); // Begin first PZEM serial
}

void loop() {
  // put your main code here, to run repeatedly:
  gridMeasure();
}
void gridMeasure() {
    measure("Grid", pzem1);
}

void measure(String label, PZEM004Tv30& pzem) {
    float volt = pzem.voltage();
    float curr = pzem.current();
    float power = pzem.power();
    float energy = pzem.energy();
    float fr = pzem.frequency();
    float pf = pzem.pf();

    if(isnan(volt)){
        Serial.println("Error reading voltage for " + label);
    } else if (isnan(curr)) {
        Serial.println("Error reading current for " + label);
    } else if (isnan(power)) {
        Serial.println("Error reading power for " + label);
    } else if (isnan(energy)) {
        Serial.println("Error reading energy for " + label);
    } else if (isnan(fr)) {
        Serial.println("Error reading frequency for " + label);
    } else if (isnan(pf)) {
        Serial.println("Error reading power factor for " + label);
    } else {
      // Print the values to the Serial console
      Serial.println(label + " Measurements:");
      Serial.print("Voltage: ");      Serial.print(volt);      Serial.println("V");
      Serial.print("Current: ");      Serial.print(curr);      Serial.println("A");
      Serial.print("Power: ");        Serial.print(power);     Serial.println("W");
      Serial.print("Energy: ");       Serial.print(energy,3);  Serial.println("kWh");
      Serial.print("Frequency: ");    Serial.print(fr, 1);     Serial.println("Hz");
      Serial.print("PF: ");           Serial.println(pf);
      Serial.println(); // Blank line for readability
    }
}