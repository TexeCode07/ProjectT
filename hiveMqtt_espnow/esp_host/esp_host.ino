#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>

#include "LittleFS.h"
#include "index.h"

#include <espnow.h>

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Search for parameter in HTTP POST request
const char* PARAM_INPUT_1 = "ssid";
const char* PARAM_INPUT_2 = "pass";

//Variables to save values from HTML form
String ssid;
String pass;

// File paths to save input values permanently
const char* ssidPath = "/ssid.txt";
const char* passPath = "/pass.txt";

// Set your Gateway IP address
IPAddress localIP;
//IPAddress localIP(192, 168, 1, 200); // hardcoded
IPAddress localGateway;
IPAddress subnet(255, 255, 0, 0);

const int ledPin = 2;
String ledState;

boolean restart = false;

int dataFlag=0;

#include "PubSubClient.h"
// MQTT Broker settings
const char* mqtt_server       = "xxxxxxxxd54dc7afc039c3636eb1b2.s1.eu.hivemq.cloud"; 
const int mqtt_port           = 8883; 
const char* mqtt_username     = "nexxxx";            // MQTT username
const char* mqtt_password     = "Nxxx@1234";        // MQTT password
const char* data12            = "daxxx2";           // MQTT topic
const char* clientID          = "daxxxReporter";    // MQTT client ID

WiFiClientSecure espClient;
PubSubClient client(espClient);

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    int  id;
    char stor[230];
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Create a structure to hold the readings from each board
struct_message board1;
struct_message board2;

// Create an array with all the structures
struct_message boardsStruct[2] = {board1, board2};

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t len) {
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);

  memcpy(&myData, incomingData, sizeof(myData));

  Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
  // Update the structures with the new incoming data
  // Correct way to copy the array:
  memcpy(boardsStruct[myData.id-1].stor, myData.stor, sizeof(myData.stor));

  Serial.printf("Stored value: %s \n", boardsStruct[myData.id-1].stor);
  Serial.println();

  dataFlag=1;

}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(200);
  while(!Serial);
  Serial.println("Ready !!!");
  
  initFS();

  // Set GPIO 2 as an OUTPUT
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  // Load values saved in LittleFS
  ssid = readFile(LittleFS, ssidPath);
  pass = readFile(LittleFS, passPath);

  ssid = removeNewlinesAndReturns(ssid);
  pass = removeNewlinesAndReturns(pass);

  // Serial.println(ssid.length());
  // Serial.println(pass.length());

  Serial.println(ssid);
  Serial.println(pass);

  if(!initWiFi()) {
    // Connect to Wi-Fi network with SSID and password
    Serial.println("Setting AP (Access Point)");
    // NULL sets an open Access Point
    WiFi.softAP("ESP-WIFI-MANAGER", NULL);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP); 

    // Web Server Root URL
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
       request->send_P(200, "text/html", index_html);
    });
    
    server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
      int params = request->params();
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          // HTTP POST ssid value
          if (p->name() == PARAM_INPUT_1) {
            ssid = p->value().c_str();
            Serial.print("SSID set to: ");
            Serial.println(ssid);
            // Write file to save value
            writeFile(LittleFS, ssidPath, ssid.c_str());
          }
          // HTTP POST pass value
          if (p->name() == PARAM_INPUT_2) {
            pass = p->value().c_str();
            Serial.print("Password set to: ");
            Serial.println(pass);
            // Write file to save value
            writeFile(LittleFS, passPath, pass.c_str());
          }
          //Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
      
      request->send(200, "text/plain", "Done. ESP will restart, connect to your router");
      delay(5000);
      ESP.restart();
    });
    server.begin();
  }

  espClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);

  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientID, mqtt_username, mqtt_password)) {  
      Serial.println("Connected to MQTT Broker!");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop(){

  // client.publish("data12","hello");
  // delay(5000);
  if(dataFlag == 1){
    char topic[255];
    snprintf(topic, 255, "%s%d", data12, myData.id);

    // PUBLISH to the MQTT Broker (topic = data12)
    if (client.publish(topic, boardsStruct[myData.id-1].stor)) {
      Serial.println("data12 sent!");
    }else {
      Serial.println("Data failed to send. Reconnecting to MQTT Broker and trying again");
      client.connect(clientID, mqtt_username, mqtt_password);
      delay(10); // This delay ensures that client.publish doesn’t clash with the client.connect call
      client.publish(topic, boardsStruct[myData.id-1].stor);
    }

    dataFlag=0;
  }

  // Access the variables for each board
  /*int board1X = boardsStruct[0].x;
  int board1Y = boardsStruct[0].y;
  int board2X = boardsStruct[1].x;
  int board2Y = boardsStruct[1].y;
  */
}

// Initialize LittleFS
void initFS() {
  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  else{
    Serial.println("LittleFS mounted successfully");
  }
}

// Read File from LittleFS
String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
    Serial.println("- failed to open file for reading");
    return String();
  }

  String fileContent;
  while(file.available()){
    fileContent = file.readStringUntil('\n');
    break;
  }
  file.close();
  return fileContent;
}

// Write file to LittleFS
void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, "w");
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- frite failed");
  }
  file.close();
}

// Initialize WiFi
bool initWiFi() {
  if(ssid==""){
    Serial.println("Undefined SSID !!");
    return false;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), pass.c_str());
  Serial.println("Connecting to WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println();
  Serial.print("IP Address: ");     Serial.println(WiFi.localIP());
  Serial.print("Wi-Fi Channel: ");  Serial.println(WiFi.channel());
  return true;
}

// Replaces placeholder with LED state value
String processor(const String& var) {
  if(var == "STATE") {
    if(!digitalRead(ledPin)) {
      ledState = "ON";
    }
    else {
      ledState = "OFF";
    }
    return ledState;
  }
  return String();
}


String removeNewlinesAndReturns(String input) {
  String result;
  for (char c : input) {
    if (c != '\n' && c != '\r') {
      result += c;
    }
  }
  return result;
}

