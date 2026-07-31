#include <WiFi.h>
#include <HTTPClient.h>

// Replace with your network credentials
const char* ssid = "onebroadband";
const char* password = "12345678";

// ThingSpeak settings
const char* server = "http://api.thingspeak.com";
String apiKey = "J4HV5ANMDLG5RGIQ"; // Your API Write Key

// Time interval to post data (in milliseconds)
unsigned long interval = 5000; // Post every 5 seconds
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);
  delay(100);
  
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("WiFi connected.");
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Generate random data for ThingSpeak fields
    int nitrogen = random(0, 100);
    int phosphorus = random(0, 100);
    int potassium = random(0, 100);
    float pH = random(0, 14);
    int moisture = random(0, 100);
    float conductivity = random(0, 100) / 10.0;

    // Create a URL string to post the data
    String url = server;
    url += "/update?api_key=" + apiKey;
    url += "&field1=" + String(nitrogen);
    url += "&field2=" + String(phosphorus);
    url += "&field3=" + String(potassium);
    url += "&field4=" + String(pH);
    url += "&field5=" + String(moisture);
    url += "&field6=" + String(conductivity);

    // Send the HTTP GET request
    HTTPClient http;
    http.begin(url);
    int httpResponseCode = http.GET();

    // Check the response code
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Data sent to ThingSpeak");
      Serial.println(response);
    } else {
      Serial.print("Error sending data: ");
      Serial.println(httpResponseCode);
    }

    http.end(); // Close the connection
  }
}
