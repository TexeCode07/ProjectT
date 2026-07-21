#include <Arduino.h>
#include <WiFi.h>

#include <AudioOutputNull.h>
#include <AudioOutputI2S.h>
#include <AudioGeneratorMIDI.h>
#include "AudioFileSourcePROGMEM.h"

#include "sf2.h"
#include "mid.h"

AudioFileSourcePROGMEM *sf2;
AudioFileSourcePROGMEM *mid;
AudioOutputI2S *dac;
AudioGeneratorMIDI *midi;

void setup()
{
  WiFi.mode(WIFI_OFF); 

  Serial.begin(115200);
  Serial.println("Starting up...\n");

  audioLogger = &Serial;
  sf2 = new AudioFileSourcePROGMEM(sf2_h, sizeof(sf2_h));
  mid = new AudioFileSourcePROGMEM(mid_h, sizeof(mid_h));
    dac = new AudioOutputI2S();
  midi = new AudioGeneratorMIDI();
  midi->SetSoundfont(sf2);
  midi->SetSampleRate(22050);
  Serial.printf("BEGIN...\n");
  midi->begin(mid, dac);
}

void loop()
{
  if (midi->isRunning()) {
    if (!midi->loop()) {
      uint32_t e = millis();
      midi->stop();
    }
  } else {
    Serial.printf("MIDI done\n");
    delay(1000);
  }
}