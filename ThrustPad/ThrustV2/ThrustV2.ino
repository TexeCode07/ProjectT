/*
 * Complete project details at https://RandomNerdTutorials.com/esp32-load-cell-hx711/
 *
 * HX711 library for Arduino - example file
 * https://github.com/bogde/HX711
 *
 * MIT License
 * (c) 2018 Bogdan Necula
 *
**/
#include <Arduino.h>
#include "HX711.h"
#include "soc/rtc.h"
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif


BluetoothSerial SerialBT;
int relPin= 26;
char a;

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 4;
const int LOADCELL_SCK_PIN = 2;

HX711 scale;

void setup() {
  Serial.begin(115200);
  // rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);
  Serial.println("HX711 Demo");

  Serial.println("Initializing the scale");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  // Serial.println("Before setting up the scale:");
  // Serial.print("read: \t\t");
  // Serial.println(scale.read());      // print a raw reading from the ADC

  // Serial.print("read average: \t\t");
  // Serial.println(scale.read_average(20));   // print the average of 20 readings from the ADC

  // Serial.print("get value: \t\t");
  // Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight (not set yet)

  // Serial.print("get units: \t\t");
  // Serial.println(scale.get_units(5), 1);  // print the average of 5 readings from the ADC minus tare weight (not set) divided
  //           // by the SCALE parameter (not set yet)
            
  scale.set_scale(490);
  //scale.set_scale(-471.497);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();               // reset the scale to 0

  Serial.println("After setting up the scale:");

  Serial.print("read: \t\t");
  Serial.println(scale.read());                 // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));       // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight, set with tare()

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided
            // by the SCALE parameter set with set_scale

  Serial.println("Readings:");

  SerialBT.begin("ESP32-ThrustPad2"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  Serial.println("After Successfully Connected");
  Serial.println("Enter 'A' in the termainal to Start Countdown...........");

  //Pin 13 is for Relay activation
  pinMode(relPin,OUTPUT);
  digitalWrite(relPin, HIGH);
  
}

void loop() {
  if (SerialBT.available()) {
    a =(char)SerialBT.read();
    Serial.println(a);
    
    if(a == 'A'){

      Serial.println("10");SerialBT.println("10");
      delay(1000);
      Serial.println("9"); SerialBT.println("9");
      delay(1000);
      Serial.println("8"); SerialBT.println("8");
      delay(1000);
      Serial.println("7"); SerialBT.println("7");
      delay(1000);
      Serial.println("6"); SerialBT.println("6");
      delay(1000);
      Serial.println("5"); SerialBT.println("5");
      delay(1000);
      Serial.println("4"); SerialBT.println("4");
      delay(1000);
      Serial.println("3"); SerialBT.println("3");
      delay(1000);
      Serial.println("2"); SerialBT.println("2");
      delay(1000);
      Serial.println("1"); SerialBT.println("1");
      delay(1000);
      Serial.println("0 IGNITION ON......................................");  SerialBT.println("0 IGNITION ON......................................");
      digitalWrite(relPin, LOW);
      delay(20000);
      digitalWrite(relPin, HIGH);      
    }
  }
//  Serial.print("Reading:\t");
  Serial.print(millis());
  Serial.print("  :  ");
  Serial.print(scale.get_units(), 1);
  Serial.println();
  // Serial.print("\t| average:\t");
  // Serial.println(scale.get_units(10), 5);

  // scale.power_down();             // put the ADC in sleep mode
  // delay(5000);
  // scale.power_up();
}
