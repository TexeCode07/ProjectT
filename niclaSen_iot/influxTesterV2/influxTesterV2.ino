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

//Credentials
#define ID "Test_5"  // Put Device ID        

#define WIFI_SSID     "xxx"  // WiFi AP SSID
#define WIFI_PASSWORD "xxx"      // WiFi password

#define INFLUXDB_URL    "https://xxxx.aws.cloud2.influxdata.com"
#define INFLUXDB_TOKEN  "xxxxB-_xZhQJBS3JIh2nvD4aH5bZwp_ps_-d5Jj5yb6kQvblyxphXkUf3Kn76UNHpDEGhsFxKrFa_w=="
#define INFLUXDB_ORG    "xxxx240b7d5d"
#define INFLUXDB_BUCKET "StructuralAlarm"

// Time zone info
#define TZ_INFO "UTC5.5"

// Declare InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Data point
Point sensor("Nicla");

void setup() {
  Serial.begin(115200);

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

    if (millis() - printTime >= 10000) {        // Adjust Time for updating values 10s Default
      printTime = millis();
      Serial.println("Magn Values");
      
      // Store measured value into point
      sensor.addField("magx", random(100));
      sensor.addField("magy", random(100));
      sensor.addField("magz", random(100));

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