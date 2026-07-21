/**
 * @file      PowefOffModem.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-11-09
 * @note      PWRKEY method power off modem example with Serial command ("on"/"off") detection.
 */

#define TINY_GSM_MODEM_SIM7600
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb
#define SerialAT Serial1

// See all AT commands, if wanted
#define DUMP_AT_COMMANDS

#include <TinyGsmClient.h>
#include "utilities.h"

#ifdef DUMP_AT_COMMANDS  // if enabled it requires the streamDebugger lib
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, Serial);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

bool modemOn = false;

void powerOnModem() {
    Serial.println("Powering on modem...");
    digitalWrite(MODEM_PWRKEY, HIGH);
    delay(300); // Need delay
    digitalWrite(MODEM_PWRKEY, LOW);

    // Check if modem responds to AT commands
    int retries = 5;
    while (retries--) {
        if (modem.testAT()) {
            Serial.println("Modem powered on successfully!");
            modemOn = true;
            return;
        }
        delay(1000);
    }

    Serial.println("Modem power on failed!");
    modemOn = false;
}

void powerOffModem() {
    Serial.println("Powering off modem...");
    digitalWrite(MODEM_PWRKEY, HIGH);
    delay(2600); // Need delay
    digitalWrite(MODEM_PWRKEY, LOW);

    // Check if modem has powered off
    int retries = 5;
    while (retries--) {
        if (!modem.testAT()) {
            Serial.println("Modem powered off successfully!");
            modemOn = false;
            return;
        }
        delay(1000);
    }

    Serial.println("Modem power off failed!");
    modemOn = true;
}

void setup() {
    Serial.begin(115200); // Set console baud rate
    SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    pinMode(MODEM_PWRKEY, OUTPUT);
    pinMode(MODEM_FLIGHT, OUTPUT);
    digitalWrite(MODEM_FLIGHT, HIGH);

    Serial.println("Type 'on' to power the modem on or 'off' to power it off.");
}

void loop() {
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        input.trim();  // Remove any extra spaces or newline characters

        if (input.equalsIgnoreCase("on")) {
            if (!modemOn) {
                powerOnModem();
            } else {
                Serial.println("Modem is already powered on.");
            }
        } else if (input.equalsIgnoreCase("off")) {
            if (modemOn) {
                powerOffModem();
            } else {
                Serial.println("Modem is already powered off.");
            }
        } else {
            Serial.println("Invalid command. Please type 'on' or 'off'.");
        }
    }

    // Small delay to avoid rapid checking
    delay(100);
}
