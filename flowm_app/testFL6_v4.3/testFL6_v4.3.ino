#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>        // Include the BLE2902 header for CCC descriptor
#include <ArduinoJson.h>    // Include the ArduinoJson library for JSON formatting
#include <Preferences.h>    // Include Preferences library for non-volatile storage
Preferences preferences; // NVS storage object

// Define the pins connected to the flow meters
const int flowMeterPins[8] = {34, 35, 32, 33, 25, 26, 27, 14};  // Adjust these pins based on your Arduino model

// Pulse counts for each flow meter
volatile unsigned long pulseCounts[8] = {0};

// Variables to calculate the flow rate
unsigned long oldTime = 0;
float flowRates[8] = {0};
float litersPerMinute[8] = {0};
unsigned long interval = 1000;  // 1 second interval for measurement

// Flow meter calibration factor (pulses per liter)
float calibrationFactors[8] = {2580.0, 2600.0, 2550.0, 2590.0, 2570.0, 2610.0, 2560.0, 2585.0};
float defaultCalibrationFactors[8] = {2580.0, 2600.0, 2550.0, 2590.0, 2570.0, 2610.0, 2560.0, 2585.0};

bool deviceConnected = false;
bool oldDeviceConnected = false;

// Define UUIDs for the service and characteristics
#define SERVICE_UUID              "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHAR_JSON_UUID            "beb5483e-36e1-4688-b7f5-ea07361b2697"
#define CALIBRATION_CHAR_UUID     "beb5483e-36e1-4688-b7f5-ea07361b2699"  // New UUID for calibration factors

// Create a BLE server and characteristics
BLEServer *pServer;
BLECharacteristic *pCharacteristicJSON;
BLECharacteristic *pPumpChar;
BLECharacteristic *pCalibrationChar;  // New BLE characteristic for calibration factors



void loadCalibrationFactors() {
  preferences.begin("calib-factors", true);  // Open NVS namespace in read-only mode
  for (int i = 0; i < 8; i++) {
    float storedValue = preferences.getFloat(("factor" + String(i + 1)).c_str(), -1);  // Read from NVS
    if (storedValue == -1) {
      // Use default value if no valid stored value is found
      calibrationFactors[i] = defaultCalibrationFactors[i];
    } else {
      calibrationFactors[i] = storedValue;
    }
  }
  preferences.end();
  Serial.println("Calibration factors loaded from NVS.");
}


void saveCalibrationFactors() {
  // Store calibration factors in NVS
  preferences.begin("calib-factors", false);  // Open NVS namespace
  for (int i = 0; i < 8; i++) {
    preferences.putFloat(("factor" + String(i + 1)).c_str(), calibrationFactors[i]);
  }
  preferences.end();
  Serial.println("Calibration factors saved to NVS.");
}


// Server callbacks to handle client connection and disconnection
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    Serial.println("Client connected");
    deviceConnected = true;
  }

  void onDisconnect(BLEServer* pServer) {
    Serial.println("Client disconnected");
    deviceConnected = false;
  }
};


// Calibration characteristic callback to handle read and write requests
class CalibrationCharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic *pCharacteristic) {
    // Create a JSON object with the current calibration factors
    StaticJsonDocument<256> jsonDoc;
    for (int i = 0; i < 8; i++) {
      jsonDoc["factor" + String(i + 1)] = calibrationFactors[i];
    }

    // Serialize the JSON to a string and set it as the characteristic's value
    String jsonString;
    serializeJson(jsonDoc, jsonString);
    pCharacteristic->setValue(jsonString.c_str());

    Serial.printf("Read calibration factors: %s\n", jsonString.c_str());
  }

  void onWrite(BLECharacteristic *pCharacteristic) {
    String value = pCharacteristic->getValue();
    if (value.length() > 0) {
      Serial.printf("Calibration written: %s\n", value.c_str());

      // Parse the JSON data received
      StaticJsonDocument<256> jsonDoc;
      DeserializationError error = deserializeJson(jsonDoc, value);
      if (!error) {
        // Update each calibration factor with the new values from the JSON
        for (int i = 0; i < 8; i++) {
          calibrationFactors[i] = jsonDoc["factor" + String(i + 1)] | calibrationFactors[i];  // Update only if key exists
        }
        
        Serial.println("Updated calibration factors.");
        saveCalibrationFactors();  // Save updated calibration factors to NVS
      } else {
        Serial.println("Failed to parse JSON for calibration factors.");
      }
    }
  }
};


void setup() {
  // Initialize the serial communication
  Serial.begin(115200);

  // Load calibration factors from NVS
  loadCalibrationFactors();

  // Set up each flow meter pin as an input and attach interrupts
  pinMode(flowMeterPins[0], INPUT);
  attachInterrupt(digitalPinToInterrupt(flowMeterPins[0]), pulseCounter0, RISING);
  
  pinMode(flowMeterPins[1], INPUT);
  attachInterrupt(digitalPinToInterrupt(flowMeterPins[1]), pulseCounter1, RISING);
  
  pinMode(flowMeterPins[2], INPUT);
  attachInterrupt(digitalPinToInterrupt(flowMeterPins[2]), pulseCounter2, RISING);
  
  pinMode(flowMeterPins[3], INPUT);
  attachInterrupt(digitalPinToInterrupt(flowMeterPins[3]), pulseCounter3, RISING);
  
  pinMode(flowMeterPins[4], INPUT);
  attachInterrupt(digitalPinToInterrupt(flowMeterPins[4]), pulseCounter4, RISING);
  
  pinMode(flowMeterPins[5], INPUT);
  attachInterrupt(digitalPinToInterrupt(flowMeterPins[5]), pulseCounter5, RISING);
  
  pinMode(flowMeterPins[6], INPUT);
  attachInterrupt(digitalPinToInterrupt(flowMeterPins[6]), pulseCounter6, RISING);
  
  pinMode(flowMeterPins[7], INPUT);
  attachInterrupt(digitalPinToInterrupt(flowMeterPins[7]), pulseCounter7, RISING);

  // Initialize BLE
  BLEDevice::init("ESP32_BLE");

  // Create a BLE server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create a BLE service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create BLE characteristic for JSON data
  pCharacteristicJSON = pService->createCharacteristic(
    CHAR_JSON_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );
  pCharacteristicJSON->addDescriptor(new BLE2902());  // Add CCC descriptor

  // Create BLE characteristic for calibration factors
  pCalibrationChar = pService->createCharacteristic(
    CALIBRATION_CHAR_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR
  );
  pCalibrationChar->setCallbacks(new CalibrationCharacteristicCallbacks());  // Set the callback for handling calibration factors

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  Serial.println("BLE server started, advertising...");
}

void loop() {
  // Handle Bluetooth connection state changes
  if (!deviceConnected && oldDeviceConnected) {
    delay(500);  // Give the Bluetooth stack the chance to get things ready
    pServer->startAdvertising();  // Restart advertising
    Serial.println("Start advertising");
    oldDeviceConnected = deviceConnected;
  }

  unsigned long currentTime = millis();
  
  // Every second, calculate the flow rates
  if (currentTime - oldTime >= interval) {
    for (int i = 0; i < 8; i++) {
      // Calculate the flow rate in L/min for each flow meter
      flowRates[i] = (pulseCounts[i] / calibrationFactors[i]) * (60000.0 / interval);  // 60000 ms in 1 min
      litersPerMinute[i] = flowRates[i];

      // Print the flow rate for each flow meter to the Serial Monitor
      // Serial.print("Flow meter ");
      // Serial.print(i + 1);
      // Serial.print(": ");
      // Serial.print(litersPerMinute[i]);
      // Serial.println(" L/min");

      // Reset the pulse count for the next measurement
      pulseCounts[i] = 0;
    }

    // Reset the timer
    oldTime = currentTime;

    // Create JSON object
    StaticJsonDocument<256> jsonDoc;
    for (int i = 0; i < 8; i++) {
      jsonDoc["Flowmeter " + String(i + 1)] = litersPerMinute[i];
      // jsonDoc["Flowmeter " + String(i + 1)] = random(250);   // send 
    }

    // Serialize JSON to string
    String jsonString;
    serializeJson(jsonDoc, jsonString);

    // Update characteristic with JSON data
    pCharacteristicJSON->setValue(jsonString.c_str());
    pCharacteristicJSON->notify();

    Serial.printf("Broadcasted JSON: %s\n", jsonString.c_str());
  }

  // Handle device reconnection
  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
  }
}
// Interrupt service routines for each flow meter
void pulseCounter0() { pulseCounts[0]++; }
void pulseCounter1() { pulseCounts[1]++; }
void pulseCounter2() { pulseCounts[2]++; }
void pulseCounter3() { pulseCounts[3]++; }
void pulseCounter4() { pulseCounts[4]++; }
void pulseCounter5() { pulseCounts[5]++; }
void pulseCounter6() { pulseCounts[6]++; }
void pulseCounter7() { pulseCounts[7]++; }



