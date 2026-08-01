/*
    Esp32 Store Magnetometer Values to InfluxDB and Then Grafana get value from InfluxDB to Dashboard
    Influx Strucutre: 
      Bucket: StructuralAlarm -> Nicla (Point) -> ESP32 -> DeviceID -> magx, magy, magz 
*/
#if defined(ESP32)
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#define DEVICE "ESP8266"
#endif

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

#include "Arduino_BHY2Host.h"

SensorXYZ magnM(SENSOR_ID_MAG);

#define ID "Test_4"  // Put Device ID        

#define WIFI_SSID     "RPrimeAnnex"   // WiFi AP SSID
#define WIFI_PASSWORD "indianola"     // WiFi password

// InfluxDB v2 server url, e.g. https://eu-central-1-1.aws.cloud2.influxdata.com (Use: InfluxDB UI -> Load Data -> Client Libraries)
#define INFLUXDB_URL "http://structuralalarm.com:8086"
// InfluxDB v2 server or cloud API token (Use: InfluxDB UI -> Data -> API Tokens -> Generate API Token)
#define INFLUXDB_TOKEN "rda8hK7gvTP52aqOmULYMG-6ZXp-7rb9yXY0_A-Xg1-wGCOLqGkS4CHXGKnDte3aoFV9juHo9M6-TDjJoLhT6w=="
// InfluxDB v2 organization id (Use: InfluxDB UI -> User -> About -> Common Ids )
#define INFLUXDB_ORG "RPrimeLabs"
// InfluxDB v2 bucket name (Use: InfluxDB UI ->  Data -> Buckets)
#define INFLUXDB_BUCKET "StructuralAlarm"

//  Set timezone string according to https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
//  Examples:
//  Pacific Time: "PST8PDT"
//  Eastern: "EST5EDT"
//  Japanesse: "JST-9"
//  Central Europe: "CET-1CEST,M3.5.0,M10.5.0/3"
#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"

// InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Data point
Point sensor("Nicla");

void setup() {
    Serial.begin(115200);

    BHY2Host.begin(false, NICLA_AS_SHIELD);
    magnM.begin();

    WiFi.disconnect();
    delay(2000);

    // Setup wifi
    WiFi.mode(WIFI_STA);
    wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to wifi");
    while (wifiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    // Add tags
    sensor.addTag("device", DEVICE);
    sensor.addTag("Id",     ID);    

    // Accurate time is necessary for certificate validation and writing in batches
    // For the fastest time sync find NTP servers in your area: https://www.pool.ntp.org/zone/
    // Syncing progress and the time will be printed to Serial.
    timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

    // Check server connection
    if (client.validateConnection()) {
        Serial.print("Connected to InfluxDB: ");
        Serial.println(client.getServerUrl());
    } else {
        Serial.print("InfluxDB connection failed: ");
        Serial.println(client.getLastErrorMessage());
    }
}

void loop() {
    if (wifiMulti.run() == WL_CONNECTED) {
      // Clear fields for reusing the point. Tags will remain untouched
      sensor.clearFields();

      static auto printTime = millis();
      BHY2Host.update();

      if (millis() - printTime >= 10000) {        // Adjust Time for updating values 10s Default
          printTime = millis();
          Serial.println(String("Nicla magn values: ") + magnM.toString());

          // Store measured value into point
          sensor.addField("magx", magnM.x());
          sensor.addField("magy", magnM.y());
          sensor.addField("magz", magnM.z());

          // Print what are we exactly writing
          Serial.print("Writing: ");
          Serial.println(sensor.toLineProtocol());

          // Check WiFi connection and reconnect if needed
          if (wifiMulti.run() != WL_CONNECTED) {
              Serial.println("Wifi connection lost");
          }

          // Write point
          if (!client.writePoint(sensor)) {
              Serial.print("InfluxDB write failed: ");
              Serial.println(client.getLastErrorMessage());
          }
        }
    }else{
        Serial.print("WiFi Lost Restarting....");
        ESP.restart();
    }
}