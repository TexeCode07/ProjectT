#include <Arduino.h>
#include <SPI.h>
#include <AudioFileSourceSD.h>
#include <AudioGeneratorMP3.h>
#include "AudioOutputI2SNoDAC.h"

// SD card and MP3 player variables
AudioGeneratorMP3 *mp3;
AudioFileSourceSD *file;
AudioOutputI2SNoDAC *out;

// SD card CS pin
void setup() {
  Serial.begin(115200);

  // Initialize SD card
  if (!SD.begin()) {
    Serial.println("SD Card initialization failed!");
    return;
  }
  Serial.println("SD Card initialized.");
  audioLogger = &Serial;
  // Initialize the audio output
  out = new AudioOutputI2SNoDAC();
  out->SetPinout(0,0,44);
  file = new AudioFileSourceSD("/infinite-bass-loop-162623.mp3");  // Replace with your MP3 file name
  mp3 = new AudioGeneratorMP3();
  mp3->begin(file, out);
}

void loop() {
  // Keep the MP3 playing
  if (mp3->isRunning()) {
    if (!mp3->loop()) {
      mp3->stop();
    }
  } else {
    Serial.println("MP3 playback stopped.");
    delay(1000);  // Prevent spamming the Serial Monitor
  }
}
