/*
  PROBABALLISTIQ_DRUMMER_1
  hh spring 2021 HANNES@EARWEEGO.NET
  based on: DMI course examples, WS 2020-21, by@ SRH marco.kuhn@srh.de
  clock based on code from 2kohm:
  https://github.com/2kohm/Teensy-USB-MIDI-CLOCK/blob/master/Teensy_midi.ino

  ver 2.5 adds
   - swing
   - amends the struct,
   - simplifies the MidiCC setter.
   - MIDI Out to note events
  ver 2.75 adds
  - TeensyTimerTool for swing:
  ver 2.76 adds
  - TeensyTimerTool for metro, NO>
  - Chrono library for Metro
  - fixed pool of rand seeds at setup!
  -- use later in loop: each step increments the rand seed PER STEP.

  var 2.8:
  - split in several .ino's
  - not yet implemented: roll: call func recursively, pass decrementing cnt arg: TeensyTimerTool https://github.com/luni64/TeensyTimerTool/wiki/Callbacks "It gets more interesting when you need to pass context to a callback function"

  var 3:

  // todo:

  ====================================
  I/F
  - oled: https://github.com/adafruit/Adafruit_SSD1306 or waveshare's "OLED_Driver.h"

  - HW pots:
  -- thresh = determinacy
  - probDev = indeterminacy
  - encoder: encode
  -- seed (default)
  -- bpm
  -- amp
  -- numSteps
  -- swing
  -- play
  midi_type* midimap[] = { &_amp, &_BPM, &_numSteps, &_thresh, &_probDev, &_varSeed, &_seqPlay, &_swing };
  ====================================
  GM drum sounds:
  35 : kick A
  36 : kick E
  40 : E snare
  39 : Clap
  42 : HH cl
  46 : HH op
  ====================================
  MIDI MAPPINGS : - not enough power available from Teensy/USB
  nanoKtl2  // single scene: all on chan 0
  \sl: 0..7
  \kn: 16..23
  butR: 64-71
  butM: 48-55
  butR: 32-39

  icon:
  chans 0..8: sl: cc13; kn cc 12; bBot: note 26, bTop: note 27

  nanoKtl  // scene 1: all on chan 0
  # 1 2 3 4 5 6 7 8 9
  \kn (14                ..               22)
  \sl 2,  3,  4,  5,  6,  8,  9,  12, 13
  \bu (23                ..                31)
  \bd (33               ..                 41)

  e.o. MIDI MAPPINGS
  ====================================

*/

// tweak vars - make analog ctls and MIDI :) //////////////////////////////////////////////
//float amp = 0.8;   // main vol || range 0..1
int BPM = 95;       // set to ridiculously low (1) when using MIDI clock  || range 60..187 == MIDI ints
bool seqPlay = true;  // play / pause   || range 0..1

// MIDI note nums for MIDI riggers. Change them if u prefer them differently ordered, e.g. 60..63
const byte notenums[] = {42, 39, 40, 36}; // GM MIDI // ﻿ hat,  clap,  snare, kick;
// midi cc nums for params. how the hell to organzie this flexible when not using a key-based dict??
byte ccnumMap[] = {7, 8, 9, 10, 11, 12, 13, 14};

// end tweak vars ////////////////////////////////////////////////////////////////////////

// MIDI implementation as struct, so cc receiver is simple:
// struct https://arduino.stackexchange.com/questions/72044/declaring-and-using-array-of-structures-in-arduino
struct midi_type {
  String ctlName ;
  byte ccnum;
  float range[2];
  float outVal;
  float oldOutVal;
  byte inVal;
  byte oldInVal;
  float curve;
  float stepSize; // emulating SC ControlSpecs
};

/// direct pots:
struct midi_type _thresh = { "thresh", 10, {0, 1}, random(50) / 100.0, 0, 0, 0, 0, 1.0 / 127 };
struct midi_type _probDev = { "probDev", 11, {0, 2}, random(50) / 100.0, 0, 0, 0,  0, 1.0 / 127 };
// on encoder
// no Button / default
struct midi_type _varSeed = { "varSeed", 12, {1, 64}, 74, 0, 0, 0, 0, 1 };
// buttons 1..7
struct midi_type _rollProb = {"rollProb", 15, {0, 100}, 5, 0, 0, 0, -3, 1};
struct midi_type _amp = { "amp", 7, {0, 1}, 0.3, 0, 0, 0,  -2,  1.0 / 127};
struct midi_type _numSteps = { "numSteps", 9, {1, 24}, 12, 0, 0, 0,  0, 1 };
struct midi_type _BPM = { "BPM", 8, {60, 187}, 95, 0, 0, 0,   0, 1 };
struct midi_type _swing = { "swing", 14, {0, 0.95}, 0.1, 0, 0, 0, -4, 0.05 };
struct midi_type _skipTo = {"skipTo", 16, {0, 127}, 63, 0, 0, 0, 0, 1};
struct midi_type _seqPlay = { "seqPlay", 13, {0, 1}, 1, 0, 0, 0, 0, 1 };

const byte numCCs = 10; // can read this from size of ccnumMap
// simple map for cc-> indices; needs hand knit reverse lookup func _indexOf
//byte ccnumMap[numCCs] = {7, 8, 9, 10, 11, 12, 13, 14};
// list 'em in an array of pointers
midi_type* midimap[] = { &_amp, &_BPM, &_numSteps, &_thresh, &_probDev, &_varSeed, &_seqPlay, &_swing, &_rollProb, &_skipTo };

#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;
//#include <Metro.h>   // https://www.pjrc.com/teensy/td_libs_Metro.html
// INCLUDE CHRONO LIBRARY : http://github.com/SofaPirate/Chrono
#include <Chrono.h>
#include <Bounce.h>
/*
  #include <algorithm> // wow, advanced C++ lib!
  #include <vector>
*/

// WAV files converted to code by wav2sketch - 1st 3 from Paul's Example, rest by AI
#include "AudioSampleSnare.h"        // http://www.freesound.org/people/KEVOY/sounds/82583/
#include "AudioSampleHihat.h"        // http://www.freesound.org/people/mhc/sounds/102790/
#include "AudioSampleKick.h"         // http://www.freesound.org/people/DWSD/sounds/171104/
#include "AudioSampleDt01_clap_hipclap16.h"
#include "AudioSampleDt01_clap_stereo99_16.h"

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioPlayMemory          sound5;         //xy=515.5666656494141,309
AudioPlayMemory          sound2;         //xy=517.566650390625,190
AudioPlayMemory          sound3;         //xy=518.566650390625,231
AudioPlayMemory          sound4;         //xy=518.5666656494141,271
AudioPlayMemory          sound1;         //xy=519.566650390625,153
AudioPlayMemory          sound0;         //xy=520.566650390625,112.99999237060547
AudioMixer4              mix1;           //xy=726.566650390625,252
AudioEffectFreeverbStereo freeverbs1;     //xy=875.2333374023438,348.23333740234375
AudioMixer4              mix2;         //xy=1049.2332153320312,239.23333740234375
AudioMixer4              mix3;         //xy=1050.2332763671875,308.23333740234375
AudioOutputAnalog        dac;            //xy=1185.5665283203125,204
AudioOutputI2S           headphones;     //xy=1208.5665283203125,263
AudioConnection          patchCord1(sound2, 0, mix1, 2);
AudioConnection          patchCord2(sound3, 0, mix1, 3);
AudioConnection          patchCord3(sound1, 0, mix1, 1);
AudioConnection          patchCord4(sound0, 0, mix1, 0);
AudioConnection          patchCord5(mix1, freeverbs1);
AudioConnection          patchCord6(mix1, 0, mix2, 0);
AudioConnection          patchCord7(mix1, 0, mix3, 0);
AudioConnection          patchCord8(freeverbs1, 0, mix2, 1);
AudioConnection          patchCord9(freeverbs1, 1, mix3, 1);
AudioConnection          patchCord10(mix2, 0, headphones, 0);
AudioConnection          patchCord11(mix2, dac);
AudioConnection          patchCord12(mix3, 0, headphones, 1);
AudioControlSGTL5000     audioShield;    //xy=1205.5665893554688,312
// GUItool: end automatically generated code

// ************************* END – GENERATED BY AUDIO DESIGN TOOL

OneShotTimer swinger; // generate a timer from the pool (Pool: 2xGPT, 16xTMR(QUAD), 20xTCK)
//Timer metro; // generate a timer from the pool (Pool: 2xGPT, 16xTMR(QUAD), 20xTCK)
//Metro metro = Metro(300);
Chrono metro;
Chrono roller;

const byte ledPin = 13;
const byte rows = 4; // 4 'tracks'; rows in the seq matrix
const byte columns = 16; // 16 steps given
// ﻿ hat,  clap,  snare, kick;
float steps[rows][columns] = {
  {0.6, 0.1, 0.95, 0.35,   0.7, 0.1, 0.92, 0.4, 0.6, 0.1, 0.95, 0.3, 0.66, 0.1, 0.9, 0.4, },
  {0, 0, 0, 0, 0.99, 0, 0.1, 0.6, 0, 0, 0, 0.33, 1, 0.11, 0, 0.3},
  {0.1, 0, 0.5, 0, 0.9, 0.6, 0.2, 0, 0, 0.3, 0.6, 0.2, 0.9, 0.1, 0.2, 0.1},
  {1.0, 0., 0., 0.7, 0., 0., 0.5, 0., 0.9, 0, 0.3, 0, 0, 0.4, 0.3, 0.2}
};
byte midi_clock_counter = 0;
byte midi_clockdivider = 6;
int msPerBeat = 0; // calc how many ms is one beat
int seqStep = 0;
int prevSeqStep = 1;
int barCount = 0;
int fillSeed = 0;
int delayTime = 0; // for the swing offset
elapsedMillis snare_env_ms;
int randSeedPool[1024];


void setup() {
  // init a big pool with rand nrs.
  randomSeed(1974);
  for (int i = 0; i < 1024; i++) {
    randSeedPool[i] = random(2147483647); // this shd be the long's range..
  }

  AudioMemory(20);
  audioShield.enable();
  audioShield.volume(_amp.outVal);

  // reduce the gain on mixer channels, so more than 1
  // sound can play simultaneously without clipping
  mix1.gain(0, 0.2);  // hh
  mix1.gain(1, 0.9);  // clp
  mix1.gain(2, 0.5);  // snr
  mix1.gain(3, 0.75);  // kiq

  mix2.gain(1, 0.25);
  mix3.gain(1, 0.25);

  swinger.begin(playStep);
  //    metro.beginPeriodic(callback, convertBPMtoMS(BPM * 4)); // 250ms // TeensyTimer
  //  metro.interval(convertBPMtoMS(BPM * 4)); // step time = 4 * BPM time
  convertBPMtoMS(BPM * 4); // Chrono time is global

  usbMIDI.setHandleControlChange(getControlChangeMsg);
  usbMIDI.setHandleRealTimeSystem(RealTimeSystem);
  Serial.begin(115200);
  Serial.println("PROBABALLISTIQ DRUMMER 1 \nHH 2021 hannes@earweego.net\nwelcome! \n\n");
}

void loop() {
  usbMIDI.read();

  // end of bar
  if (seqStep >= int(_numSteps.outVal) ) {

    seqStep = 0;
    barCount++;
    digitalWrite(ledPin, 1);

    //    randomSeed( analogRead(1) );
    randomSeed( randSeedPool[barCount % 1024] );
    // _thresh.outVal = float(random(1, 99)) / 100.0;
    // every 8 bars
    if ( barCount % 8 == random(6, 8)) {
      fillSeed = random(100);
    } else {
      fillSeed = 0;
    };
    randomSeed( _varSeed.outVal + fillSeed );
  }

  //    if (snare_env_ms > 60) {
  //      envelope1.noteOff();
  //    }

  if (metro.hasPassed(msPerBeat)) {
    metro.restart();

    // Play Pattern
    if (seqPlay) {
      if (seqStep % 2 == 1) {
        delayTime = (msPerBeat * _swing.outVal) + 1;
      } else {
        delayTime = 1;
        digitalWrite(ledPin, !digitalRead(ledPin));
      }

      //      Serial.println(seqStep);
      swinger.trigger(delayTime * 1000); // trigger the callback func playMyNote

      snare_env_ms = 0; // use for midi noteOFF ?
      prevSeqStep = seqStep;
    }

    seqStep++;
  }

}
