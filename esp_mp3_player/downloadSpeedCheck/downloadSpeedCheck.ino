#ifdef ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#else
#error "Unsupported platform"
#endif

// Wi-Fi credentials
const char* ssid = "onebroadband";        // Replace with your Wi-Fi SSID
const char* password = "12345678";        // Replace with your Wi-Fi password
// const char* fileDownloadUrl = "http://ernd.tech/test/sound/sample-2.mp3";
const char* fileDownloadUrl = "http://download.httpwatch.com/httpwatch.exe";
// Wi-Fi and HTTP settings
#define MAX_BUFFER_SIZE 6000  // Increase buffer size for faster reads

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    
    Serial.println("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting...");
    }
    Serial.println("Connected to WiFi");

    downloadFile();
}

void loop() {
    // Nothing to do here
}

void downloadFile() {
    WiFiClient client;  // Create a WiFiClient object
    HTTPClient http;
    http.setTimeout(5000); // Set timeout to prevent long waits
    http.begin(client, fileDownloadUrl); // Pass the client and URL

    int httpCode = http.GET();
    
    if (httpCode == HTTP_CODE_OK) {
        size_t fileSize = http.getSize();
        Serial.printf("Starting file download... File size: %f MB\n", fileSize / 1024.0 / 1024.0);

        uint8_t *buff = (uint8_t *)malloc(MAX_BUFFER_SIZE); // Allocate buffer
        
        if (!buff) {
            Serial.println("Failed to allocate memory for buffer");
            http.end();
            return;
        }

        unsigned long cycleTime = millis();
        size_t downloadedBytes = 0;
        size_t lastDownloadedBytes = 0; // Bytes downloaded in the last second

        while (http.connected() && (fileSize > 0 || fileSize == -1)) {
            size_t availableBytes = client.available();
            if (availableBytes > 0) {
                int bytesRead = client.readBytes(buff, min(static_cast<size_t>(MAX_BUFFER_SIZE), availableBytes));
                downloadedBytes += bytesRead;
                lastDownloadedBytes += bytesRead;

                // Update download speed every second
                if (millis() - cycleTime >= 1000) {
                    float downloadSpeed = lastDownloadedBytes / 1024.0; // Speed in KB/s
                    Serial.printf("Download speed: %f KB/s, Remaining file size: %f MB\n",
                                  downloadSpeed, (fileSize - downloadedBytes) / 1024.0 / 1024.0);
                    
                    // Reset lastDownloadedBytes for the next interval
                    lastDownloadedBytes = 0; 
                    cycleTime = millis();
                }
            }
        }

        free(buff); // Free allocated memory
        Serial.println("Download completed!");
    } else {
        Serial.printf("Failed to download, error code: %d\n", httpCode);
    }

    http.end();
}
