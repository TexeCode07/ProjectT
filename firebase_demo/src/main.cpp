#include <Arduino.h>

#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define GREEN_LED 2 

// 🔧 Change Device ID here
#define DEVICE_ID "NFC-2655"
String basePath = String("/Devices/") + DEVICE_ID;

#define WIFI_SSID     "xxxx"
#define WIFI_PASSWORD "xxx"
#define API_KEY       "xxx"
#define DATABASE_URL  "https://xxx-xxx-default-rtdb.firebaseio.com"

NoAuth no_auth;

void processData(AsyncResult &aResult);
void set_await();

using AsyncClient = AsyncClientClass;
SSL_CLIENT ssl_client, stream_ssl_client;
AsyncClient aClient(ssl_client), streamClient(stream_ssl_client);

FirebaseApp app;
RealtimeDatabase Database;
AsyncResult databaseResult;

bool taskComplete = false;

void startFingerprintEnrollment()
{
    Serial.println(">> Fingerprint enrollment started...");
    // Call RS302 fingerprint enrollment here
}

void stopFingerprintEnrollment()
{
    Serial.println(">> Fingerprint enrollment stopped.");
    // Stop RS302 fingerprint operations here
}

void processData(AsyncResult &aResult){
    // Exits when no result is available when calling from the loop.
    if (!aResult.isResult()){
        return;
    }
    if (aResult.isEvent()){
        Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.eventLog().message().c_str(), aResult.eventLog().code());
    }
    if (aResult.isDebug()){
        Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());
    }
    if (aResult.isError()){
        Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());
    }
    if (aResult.available()){
        RealtimeDatabaseResult &stream = aResult.to<RealtimeDatabaseResult>();

        if (stream.isStream())
        {
            Serial.println("----------------------------");
            Firebase.printf("event: %s\n", stream.event().c_str());
            Firebase.printf("path: %s\n", stream.dataPath().c_str());
            Firebase.printf("data: %s\n", stream.to<const char *>());

            // Convert to boolean
            bool addFP = stream.to<bool>();
            if (addFP)
            {
                Serial.println("Enrollment mode ENABLED ✅");
                digitalWrite(GREEN_LED, HIGH);
                startFingerprintEnrollment();
            }
            else
            {
                Serial.println("Enrollment mode DISABLED ❌");
                digitalWrite(GREEN_LED, LOW);
                stopFingerprintEnrollment();
            }
        }
    }

}

void show_status(bool status){
    if (status){
        Serial.println("Success");
    }else{
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
    }
}

void set_Device_IfNotExist() {
    String devicePath = String("Devices/") + DEVICE_ID;
    AsyncResult result;

    // Perform GET
    Database.get(aClient, devicePath, result);

    if (!result.available() || String(result.c_str()) == "null") {
        Serial.println("Device does not exist, creating new structure...");

        bool status;
        JsonWriter writer;
        object_t ts_json, deviceNode;

        // Firebase server timestamp
        writer.create(ts_json, ".sv", "timestamp");

        // Build default structure
        object_t obj1, obj2, obj3, obj4;
        writer.create(obj1, "Last_Seen", ts_json);
        writer.create(obj2, "User_Count", 0);
        writer.create(obj3, "Button_Press_Count", 0);
        writer.create(obj4, "addFP", false);

        // Combine all into one JSON object
        writer.join(deviceNode, 4, obj1, obj2, obj3, obj4);

        // ✅ Only update this device path (not whole Devices root!)
        status = Database.update<object_t>(aClient, devicePath, deviceNode);
        show_status(status);
    } else {
        Serial.println("✅ Device already exists, skipping creation");
    }
}

void update_Timestamp()
{
    object_t ts_json;
    JsonWriter writer;
    writer.create(ts_json, ".sv", "timestamp");

    bool status = Database.set<object_t>(
        aClient,
        "Devices/" + String(DEVICE_ID) + "/Last_Seen",
        ts_json
    );
    show_status(status);
}

void setup(){
    Serial.begin(115200);
    
    pinMode(GREEN_LED, OUTPUT);
    digitalWrite(GREEN_LED, LOW);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    set_ssl_client_insecure_and_buffer(ssl_client);
    set_ssl_client_insecure_and_buffer(stream_ssl_client);

    Serial.println("Initializing app...");
    initializeApp(aClient, app, getAuth(no_auth));

    app.getApp<RealtimeDatabase>(Database);

    Database.url(DATABASE_URL);

    // Check and set structure only once at boot
    set_Device_IfNotExist();

    // Start listening to /Devices/NFC-2662/addFP
    String path = "/Devices/" + String(DEVICE_ID) + "/addFP";
    Database.get(streamClient, path, processData, true /* SSE mode */, "streamTask");

}

void loop() {
    app.loop();
    processData(databaseResult);

    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 5000) { // update every 5s
        update_Timestamp();
        lastUpdate = millis();
    }
}
