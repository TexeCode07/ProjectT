#include <FS.h>
#include <LittleFS.h> 
#include <RTClib.h>
#include <ArduinoJson.h>
RTC_Millis rtc;
int count=0;

#include <ESP8266WiFi.h>
#include <espnow.h>
// REPLACE WITH RECEIVER MAC Address
// uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t broadcastAddress[] = {0xb4, 0x8a, 0x0a, 0xe2, 0x66, 0x61};
// Set your Board ID (ESP32 Sender #1 = BOARD_ID 1, ESP32 Sender #2 = BOARD_ID 2, etc)
#define BOARD_ID 2
// Must match the receiver structure
typedef struct struct_message {
    int id;
    char stor[230];
} struct_message;
// Create a struct_message called test to store variables to be sent
struct_message myData;
String line;

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;  // This is trying to send 

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("\r\nLast Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
    LittleFS.remove("/mystore.txt");
    ESP.restart();
  }
  else{
    Serial.println("Delivery fail");
   
  }
}
 

void setup() {
  Serial.begin(115200);
  delay(200);
  while(!Serial);
  Serial.println("Ready !!!");

  rtc.begin(DateTime(F(__DATE__), F(__TIME__)));

  if (!LittleFS.begin()) {
    Serial.println("An error occurred while mounting LittleFS.");
    return;
  }

  printStoredData();
  storeValueLoop();

  WiFi.mode(WIFI_STA);  // Set device as a Wi-Fi Station
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  } 
  // Set ESP-NOW role
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  // Once ESPNow is successfully init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  // Set values to send
  myData.id          = BOARD_ID;
  memset(myData.stor, 0, sizeof(myData.stor));  // Ensure the array is initially empty
  // Copy the JSON string to the char array, leaving space for a null terminator
  // Ensure not to copy more characters than the array can hold minus 1 for the null terminator
  strncpy(myData.stor, line.c_str(), sizeof(myData.stor) - 1);

}

void loop() {
  // No need for anything here as the logic is in the setup function
  if ((millis() - lastTime) > timerDelay) {    
      // Send message via ESP-NOW
      esp_now_send(0, (uint8_t *) &myData, sizeof(myData));
    
    lastTime = millis();
  }
}

void storeValueLoop(){
  if (count < 12) {
    StaticJsonDocument<200> doc; 
    doc["t"] = millis(); 
    doc["d"] = random(100); 
    String jsonString;
    serializeJson(doc, jsonString);

    File file = LittleFS.open("/mystore.txt", "a");
    if (!file) {
      Serial.println("Failed to open /mystore.txt file for appending");
      return;
    }
    file.print(jsonString); file.print(",");
    file.close();
    
    Serial.println("Entering deep sleep for 10 seconds...");
    ESP.deepSleep(2e6); // Sleep for 10 seconds (10e6 microseconds)
    // delay(2000);
  } else {
    Serial.println("All data 12 stored!!!");
  }
}

void printStoredData() {
  Serial.println("mystore data: ");
  File file = LittleFS.open("/mystore.txt", "r");
  if (!file) {
    Serial.println("Failed to open mystore.txt file for reading");
    return;
  }
  
  while (file.available()) {
    line = file.readStringUntil('\n');
  }
  Serial.println(line);
  count = countOpeningBraces(line);
  Serial.print("count->"); Serial.print(count); Serial.print(" size:"); Serial.println(file.size());
  file.close();
}

int countOpeningBraces(String input) {
  int count = 0;
  for (int i = 0; i < input.length(); i++) {
    if (input.charAt(i) == '{') {
      count++;
    }
  }
  return count;
}