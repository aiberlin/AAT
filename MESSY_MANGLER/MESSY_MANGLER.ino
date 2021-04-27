// HH version of SamplePlayer example, plays alone without external interface. can be fun, too!-) // 
// spring 2021 hannes@airborneinstruments.eu

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>

// WAV files converted to code by wav2sketch
#include "AudioSampleSnare.h"        // http://www.freesound.org/people/KEVOY/sounds/82583/
#include "AudioSampleTomtom.h"       // http://www.freesound.org/people/zgump/sounds/86334/
#include "AudioSampleHihat.h"        // http://www.freesound.org/people/mhc/sounds/102790/
#include "AudioSampleKick.h"         // http://www.freesound.org/people/DWSD/sounds/171104/
#include "AudioSampleGong.h"         // http://www.freesound.org/people/juskiddink/sounds/86773/
#include "AudioSampleCashregister.h" // http://www.freesound.org/people/kiddpark/sounds/201159/

// Create the Audio components.  These should be created in the
// order data flows, inputs/sources -> processing -> outputs
//
AudioPlayMemory    sound0;
AudioPlayMemory    sound1;  // six memory players, so we can play
AudioPlayMemory    sound2;  // all six sounds simultaneously
AudioPlayMemory    sound3;
AudioPlayMemory    sound4;
AudioPlayMemory    sound5;
AudioMixer4        mix1;    // two 4-channel mixers are needed in
AudioMixer4        mix2;    // tandem to combine 6 audio sources
AudioOutputI2S     headphones; // audio output
AudioOutputAnalog  dac;     // play to both I2S audio board and on-chip DAC
// Create Audio connections between the components
AudioConnection c1(sound0, 0, mix1, 0);
AudioConnection c2(sound1, 0, mix1, 1);
AudioConnection c3(sound2, 0, mix1, 2);
AudioConnection c4(sound3, 0, mix1, 3);
AudioConnection c5(mix1, 0, mix2, 0);   // output of mix1 into 1st input on mix2
AudioConnection c6(sound4, 0, mix2, 1);
AudioConnection c7(sound5, 0, mix2, 2);
AudioConnection c8(mix2, 0, headphones, 0);
AudioConnection c9(mix2, 0, headphones, 1);
AudioConnection c10(mix2, 0, dac, 0);
// Create an object to control the audio shield.
AudioControlSGTL5000 audioShield;


int prob = 500; // variable probability


void setup() {

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(10);

  // turn on the output
  audioShield.enable();
  audioShield.volume(0.8);

  // reduce the gain on mixer channels, so more than 1
  // sound can play simultaneously without clipping
  mix1.gain(0, 0.4);
  mix1.gain(1, 0.4);
  mix1.gain(2, 0.4);
  mix1.gain(3, 0.4);
  mix2.gain(1, 0.4);
  mix2.gain(2, 0.4);
}

void loop() {
  if (random(100) < 1) {
    prob = random(2, 1000);
  }

  if (random(prob) < 1) {
    sound0.play(AudioSampleSnare);
  }
  if (random(prob) < 1) {
    sound1.play(AudioSampleTomtom);
  }
  if (random(prob) < 1) {
    sound2.play(AudioSampleHihat);
  }
  if (random(prob) < 1) {
    sound3.play(AudioSampleKick);
  }
  if (random(prob) < 1) {
    // comment this line to work with Teensy 3.0; the Gong sound is very long, too much for 3.0's memory
    sound4.play(AudioSampleGong);
  }
  if (random(prob) < 1) {
    sound5.play(AudioSampleCashregister);
  }

  if (random(prob) < 1) { sound0.stop(); }
  if (random(prob) < 1) { sound1.stop(); }
  if (random(prob) < 1) { sound2.stop(); }
  
  delay(10);
}
