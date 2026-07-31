#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define BLACK 0x0000

#define TFT_RST   25    
#define TFT_CS    5     
#define TFT_DC    26    

#include <Adafruit_GFX.h>      
#include <Adafruit_ST7735.h>   
#include <SPI.h>
#include <qrcode_st7735.h>

// Replace these with your Wi-Fi credentials
const char* ssid      = "onebroadband";
const char* password  = "12345678";

// URL for the HTTP request
const char* url = "http://164.160.40.184:8061/api/bornes/150/2020";

#define TFTMODEL INITR_144GREENTAB

Adafruit_ST7735 display = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
QRcode_ST7735 qrcode (&display);

unsigned long previousMillis = 0;
unsigned long interval = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");

  // Initialize the TFT display
  display.initR(TFTMODEL);
  Serial.println("TFT display initialized...");

  // Enable debug QR code
  qrcode.debug();

  // Initialize QR code display using the library
  qrcode.init();
  Serial.println("QR code library initialized...");

  // Connect to Wi-Fi
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Wi-Fi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Initial QR code fetch and display
  fetchAndDisplayQRCode();
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    fetchAndDisplayQRCode();
  }
}

void fetchAndDisplayQRCode() {
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();

  if (httpCode > 0) {
    // HTTP header has been sent and server response header has been handled
    Serial.printf("HTTP GET... code: %d\n", httpCode);

    // File found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println("Received payload:");
      Serial.println(payload);

      // Parse the JSON response
      StaticJsonDocument<1024> doc;
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        const char* code = doc["code"][0]["code"];
        int minute = doc["code"][0]["minute"];
        Serial.print("Extracted code: ");
        Serial.println(code);
        Serial.print("Update interval (minutes): ");
        Serial.println(minute);

        // Clear the display
        display.fillScreen(BLACK);
        
        // Create and display the QR code
        qrcode.create(code);  // http://ip+code
        Serial.println("QR code generated...");

        // Update interval (convert minutes to milliseconds)
        interval = minute  * 1000;  // note its in second so *60
      } else {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.c_str());
      }
      
    }
  } else {
    Serial.printf("HTTP GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}
