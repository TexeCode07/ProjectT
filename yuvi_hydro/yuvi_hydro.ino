#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <SD.h>

// ─── Pins ─────────────────────────────
#define DHT_PIN         26
#define TEMP_PROBE_PIN  25
#define PH_PIN          34
#define SDA_PIN         21
#define SCL_PIN         22

// SD SPI Pins
#define SD_CS   5
#define SD_SCK  18
#define SD_MOSI 23
#define SD_MISO 19

#define DHT_TYPE DHT22

// ─── pH Calibration ───────────────────
#define PH_VREF         3.3f
#define PH_SLOPE        59.16f
#define PH_MID_VOLTAGE  1.65f   // change to 2.5 if calibrated that way

// ─── Display ──────────────────────────
U8G2_SH1106_128X64_NONAME_F_HW_I2C display(
  U8G2_R0, U8X8_PIN_NONE, SCL_PIN, SDA_PIN
);

// ─── Sensors ──────────────────────────
DHT dht(DHT_PIN, DHT_TYPE);
OneWire oneWire(TEMP_PROBE_PIN);
DallasTemperature probe(&oneWire);

// ─── Variables ────────────────────────
float dhtTemp = 0, dhtHumid = 0, probeTemp = 0;
float phValue = 7.0f;

uint32_t lastRead = 0;
uint32_t lastLogTime = 0;

const uint32_t LOG_INTERVAL = 300000; // 5 min

File logFile;

// ─── pH Read ─────────────────────────
float readPH() {
  long sum = 0;

  for (int i = 0; i < 20; i++) {
    sum += analogRead(PH_PIN);
    delay(3);
  }

  float avg = sum / 20.0f;
  float voltage = avg * (PH_VREF / 4095.0f);

  // Debug
  Serial.print("ADC: "); Serial.print(avg);
  Serial.print("  Voltage: "); Serial.println(voltage);

  // Prevent invalid readings
  if (voltage <= 0.1 || voltage >= 3.2) {
    return phValue; // keep last valid value
  }

  float ph = 7.0f + (PH_MID_VOLTAGE - voltage) / (PH_SLOPE / 1000.0f);

  return constrain(ph, 0.0f, 14.0f);
}
// ─── Read Sensors ─────────────────────
void readSensors() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (!isnan(h) && !isnan(t)) {
    dhtHumid = h;
    dhtTemp = t;
  }

  probe.requestTemperatures();
  float pt = probe.getTempCByIndex(0);
  if (pt != DEVICE_DISCONNECTED_C) probeTemp = pt;

  phValue = readPH();
}

// ─── OLED Dashboard ───────────────────
void drawAllSensors() {
  display.clearBuffer();

  display.setFont(u8g2_font_6x10_tr);
  display.drawStr(25, 10, "SENSOR DATA");
  display.drawHLine(0, 12, 128);

  char buf[20];

  snprintf(buf, sizeof(buf), "Air: %.1f C", dhtTemp);
  display.drawStr(2, 25, buf);

  snprintf(buf, sizeof(buf), "Hum: %.1f %%", dhtHumid);
  display.drawStr(2, 38, buf);

  snprintf(buf, sizeof(buf), "Probe: %.1f C", probeTemp);
  display.drawStr(2, 51, buf);

  display.setFont(u8g2_font_helvR18_tr);
  snprintf(buf, sizeof(buf), "%.2f", phValue);
  display.drawStr(80, 45, buf);

  display.setFont(u8g2_font_5x7_tr);
  display.drawStr(90, 60, "pH");

  display.sendBuffer();
}

// ─── SD Init ──────────────────────────
void initSD() {
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  if (!SD.begin(SD_CS)) {
    Serial.println("❌ SD Card Failed");
    return;
  }

  Serial.println("✅ SD Card Ready");

  if (!SD.exists("/sensor_log.csv")) {
    logFile = SD.open("/sensor_log.csv", FILE_WRITE);
    if (logFile) {
      logFile.println("Time(ms),Temp,Humidity,ProbeTemp,pH");
      logFile.close();
      Serial.println("📄 CSV Created");
    }
  }
}

// ─── Log Data ─────────────────────────
void logToSD() {
  logFile = SD.open("/sensor_log.csv", FILE_APPEND);

  if (logFile) {
    logFile.print(millis());
    logFile.print(",");
    logFile.print(dhtTemp);
    logFile.print(",");
    logFile.print(dhtHumid);
    logFile.print(",");
    logFile.print(probeTemp);
    logFile.print(",");
    logFile.println(phValue);

    logFile.close();

    Serial.println("💾 Data Logged");
  } else {
    Serial.println("❌ Write Failed");
  }
}

// ─── Setup ───────────────────────────
void setup() {
  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN);
  display.begin();
  display.setContrast(200);

  analogReadResolution(12);

  display.clearBuffer();
  display.setFont(u8g2_font_6x10_tr);
  display.drawStr(20, 30, "Starting...");
  display.sendBuffer();

  dht.begin();
  probe.begin();

  initSD();

  delay(1500);
}

// ─── Loop ────────────────────────────
void loop() {
  uint32_t now = millis();

  // Read sensors every 2.5 sec
  if (now - lastRead >= 2500) {
    readSensors();
    lastRead = now;
  }

  // Log every 5 minutes
  if (now - lastLogTime >= LOG_INTERVAL) {
    logToSD();
    lastLogTime = now;
  }

  drawAllSensors();

  Serial.printf("T: %.1f  H: %.1f  P: %.1f  pH: %.2f\n",
                dhtTemp, dhtHumid, probeTemp, phValue);

  delay(100);
}