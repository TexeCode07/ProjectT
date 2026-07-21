#include "Arduino.h"
#include <Wire.h>
#include <RTClib.h>
/*

Scanning...
I2C device found at address 0x50  !
I2C device found at address 0x58  !
I2C device found at address 0x68  !
done

*/
//RelayPin
#define  relPin 2

// Note: Define the Current date and time only needed once if you set already then 
// just comment out the function rtc.adjust()
const int currentYear    = 2023;
const int currentMonth   = 12;
const int currentDay     = 9;
const int currentHour    = 15;
const int currentMinute  = 38;
const int currentSecond  = 0;

// Define the target date and time
const int targetYear    = 2023;
const int targetMonth   = 12;
const int targetDay     = 9;
const int targetHour    = 15;
const int targetMinute  = 40;
const int targetSecond  = 0;


RTC_DS1307 rtc; // Create an RTC object

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for Serial Monitor to open

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  // Comment this line if you set already no need to set again and again
  // Set the date and time here
  // rtc.adjust(DateTime(currentYear, currentMonth, currentDay, currentHour, currentMinute, currentSecond)); // Format: (year, month, day, hour, minute, second)

  // Print current time to confirm it has been set
  printTime();
  pinMode(relPin, OUTPUT);
  digitalWrite(relPin, HIGH);   // Make Relay ON
  
}

void loop() {
  DateTime now = rtc.now();
  // Compare the current time with the target time
  if (now.year() == targetYear && now.month() == targetMonth && now.day() == targetDay &&
      now.hour() == targetHour && now.minute() == targetMinute ) {
    Serial.println("Relay Trigger");
    digitalWrite(relPin, LOW);  // Make Relay OFF
  }
  printTime();
  delay(1000); // Delay to prevent checking too frequently
}

void printTime() {
  DateTime now = rtc.now();

  Serial.print("Current time: ");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);
  delay(3000);
}