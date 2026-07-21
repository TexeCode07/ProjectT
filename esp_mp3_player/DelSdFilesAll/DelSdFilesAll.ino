#include <SD.h>
#include <SPI.h>

const int SD_CS_PIN = 5;  // Change to your CS pin (commonly pin 5 for ESP32)

void deleteFilesInDir(File dir) {
    while (true) {
        File entry = dir.openNextFile();
        if (!entry) {
            // No more files in the directory
            break;
        }
        
        // If it's a directory, recursively delete its contents
        if (entry.isDirectory()) {
            Serial.print("Deleting folder: ");
            Serial.println(entry.name());
            deleteFilesInDir(entry);  // Recursive call
            SD.rmdir(entry.name());   // Delete empty directory after contents are removed
        } else {
            // It's a file, delete it
            Serial.print("Deleting file: ");
            Serial.println(entry.name());
            SD.remove(entry.name());
        }
        entry.close();
    }
}

void deleteAllFiles() {
    File root = SD.open("/");
    deleteFilesInDir(root);
    root.close();
}

void setup() {
    Serial.begin(115200);

    // Initialize SD card
    if (!SD.begin()) {
        Serial.println("SD card initialization failed!");
        return;
    }
    
    Serial.println("SD card initialized successfully.");
    
    // Start deleting all files and directories
    deleteAllFiles();
    
    Serial.println("All files and directories deleted.");
}

void loop() {
    // Do nothing after deletion
}
