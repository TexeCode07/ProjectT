#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>

// set Wi-Fi SSID and password
const char *ssid     = "SSID";
const char *password = "PASSWORD";

bool timeFormated = true;           // true = 12 hours format || false = 24 hours format
int period = 30000;                  // Update interval
unsigned long timeNow = -period;
const long utcOffsetInSeconds = 39600;
int Minute, Hour;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Connected!");
  timeClient.begin();
}

void loop() {
  while (millis() > timeNow + period) {
    timeNow = millis();
    timeClient.update();
    Minute = timeClient.getMinutes();
    Hour  = timeClient.getHours();

    if (timeFormated) {
      if (Hour >= 13)
        Hour = Hour - 12;
      if (Hour == 00)
        Hour = 12;
      else
        Hour = Hour;
    }
    else {
      Hour = Hour;
    }
    Serial.write(Hour);
    Serial.write(Minute);
  }
}
