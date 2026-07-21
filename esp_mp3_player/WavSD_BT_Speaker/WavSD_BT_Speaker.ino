#include "BluetoothA2DPSource.h"
#include <SD.h>
#include <SPI.h>

#define SD_CS_PIN 5 // Adjust this based on your hardware setup
#define WAV_FILE_PATH "/Audio/wav/test.wav" // Path to the WAV file on the SD card

BluetoothA2DPSource a2dp_source;
File wavFile;

// WAV file properties
const int WAV_BUFFER_SIZE = 512;
uint8_t wavBuffer[WAV_BUFFER_SIZE];
uint32_t wavDataStart = 0;
uint32_t wavFileSize = 0;

// Function to read WAV headers and find the data chunk
bool initializeWAV(File &file) {
    if (!file) {
        Serial.println("Failed to open WAV file");
        return false;
    }

    char chunkID[5] = {0};
    uint32_t chunkSize;

    // Read RIFF header
    file.read((uint8_t*)chunkID, 4); // Explicit cast to uint8_t*
    if (strncmp(chunkID, "RIFF", 4) != 0) {
        Serial.println("Invalid WAV file: Missing RIFF");
        return false;
    }

    file.read((uint8_t*)&chunkSize, 4); // Explicit cast to uint8_t*
    file.read((uint8_t*)chunkID, 4); // Explicit cast to uint8_t*
    if (strncmp(chunkID, "WAVE", 4) != 0) {
        Serial.println("Invalid WAV file: Missing WAVE");
        return false;
    }

    // Read chunks until we find "data"
    while (file.available()) {
        file.read((uint8_t*)chunkID, 4); // Explicit cast to uint8_t*
        file.read((uint8_t*)&chunkSize, 4); // Explicit cast to uint8_t*

        if (strncmp(chunkID, "data", 4) == 0) {
            wavDataStart = file.position();
            wavFileSize = chunkSize;
            Serial.printf("WAV Data Chunk Found at %lu with size %lu\n", wavDataStart, wavFileSize);
            return true;
        } else {
            // Skip this chunk
            file.seek(file.position() + chunkSize);
        }
    }

    Serial.println("WAV file does not contain a data chunk");
    return false;
}

// Data callback for Bluetooth streaming
int32_t get_data_frames(Frame *frame, int32_t frame_count) {
    if (!wavFile.available() || wavFile.position() >= wavDataStart + wavFileSize) {
        return 0; // No more data to stream
    }

    int bytesToRead = frame_count * sizeof(Frame);
    int bytesRead = wavFile.read((uint8_t *)frame, bytesToRead); // Explicit cast to uint8_t*

    // Fill remaining frames with silence if we reach EOF
    if (bytesRead < bytesToRead) {
        memset((uint8_t *)frame + bytesRead, 0, bytesToRead - bytesRead);
    }

    return bytesRead / sizeof(Frame);
}

void setup() {
    Serial.begin(115200);

    // Initialize SD card
    if (!SD.begin(SD_CS_PIN)) {
        Serial.println("SD card initialization failed");
        while (1);
    }

    wavFile = SD.open(WAV_FILE_PATH);
    if (!wavFile || !initializeWAV(wavFile)) {
        Serial.println("Failed to initialize WAV file");
        while (1);
    }

    a2dp_source.set_data_callback_in_frames(get_data_frames);
    a2dp_source.start("SoundDrum P");

    Serial.println("Bluetooth A2DP streaming started");
}

void loop() {
    // Prevent watchdog reset
    delay(1000);
}
