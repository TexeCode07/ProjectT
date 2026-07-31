#include <HPDL1414.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11); // TX, RX

const byte dataPins[7] = {2, 3, 4, 5, 6, 7, 8}; // Segment data pins: D0 - D6
const byte addrPins[2] = {A1, A2}; // Segment address pins: A1, A2
const byte wrenPins[] = {A0}; // Write Enable pins (left to right)

HPDL1414 hpdl(dataPins, addrPins, wrenPins, sizeof(wrenPins));

int Minute, Hour;
char Time[4];

void setup() {
  Serial.begin(115200);
  mySerial.begin(115200);
  hpdl.begin();
  hpdl.printOverflow(true);
  hpdl.clear();
  hpdl.print("----");
  delay(500);
  hpdl.clear();
}

void loop() {
  while (mySerial.available()) {
    Hour = mySerial.read();
    Minute = mySerial.read();

    sprintf(Time, "%02d%02d", Hour, Minute);
    Serial.println(Time);
    
    hpdl.clear();
    hpdl.print(Time);
  }
}
