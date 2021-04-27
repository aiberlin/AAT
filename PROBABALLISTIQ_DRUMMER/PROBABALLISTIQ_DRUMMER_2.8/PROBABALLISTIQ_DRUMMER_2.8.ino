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
  - not yet implemented: roll.

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
  float outVal;    // == default
  float oldOutVal;
  float inVal;      // 0..1 norm'd
  float oldInVal;   // 0..1 norm'd, unused
  float curve;
  float stepSize;  // emulating SC ControlSpecs & for Encoder
  byte encoderIndex;
};

/*
   nktl2:
   \sl: 0..7\
   \kn: 16..23
*/
//// direct pots; compatible to encoder pots...
struct midi_type _thresh  =  { "THRS",  0, {0, 1}, random(50) / 100.0, 0, 0, 0, 0, 1.0 / 127, 5 };
struct midi_type _probDev  = { "DEV ",  1, {0, 2}, random(50) / 100.0, 0, 0, 0,  0, 1.0 / 127, 6 };
//// on encoder
// no Button / default: enc.addr 0
struct midi_type _varSeed =  { "SEED",  16, {1, 1000}, 74, 0, 0, 0, 0, 1,          0 };
// 8 Encoders 1..8
struct midi_type _rollProb = { "ROLL",  2,  {0, 100}, 5, 0, 0, 0, -3, 2,          1 };
struct midi_type _amp =      { "VOL ",  3,  {0, 111}, 33, 0, 0, 0,  -2, 1.0 / 111,  2 };
struct midi_type _numSteps = { "STEP",  4,  {1, 24}, 12, 0, 0, 0,  0, 1,          4 };
struct midi_type _BPM =      { "BPM ",  5,  {30, 222}, 95, 0, 0, 0,   0, 1,       8 };
struct midi_type _swing =    { "SWNG",  6,  {0, 95}, 10, 0, 0, 0, -4, 5,         16 };
struct midi_type _skipTo =   { "SKIP",  7,  {0, 2}, 1, 0, 0, 0, 0, 1,            32 };
struct midi_type _seqPlay =  { "PLAY",  18, {0, 1}, 1, 0, 0, 0, 0, 1,             64 };
struct midi_type _free =     { "FREE",  17, {0, 2}, 1, 0, 0, 0, 0, 1,            128 }; // opposite of freeze. freewheelin' rand seed

const byte numCCs = 11; // still needed for now, as I can't get sizeOf of these structs...
// list 'em in an array of pointers
midi_type* midimap[] = { &_amp, &_BPM, &_numSteps, &_thresh, &_probDev, &_varSeed, &_seqPlay, &_swing, &_rollProb, &_skipTo, &_free };


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
unsigned int msPerBeat = 0; // calc how many ms is one 1/16
unsigned long usPerSubBeat = 0; // calc how many ms is 1/16/24
unsigned long barCount = 0;
int seqStep = 0;
int fillSeed = 0;
int delayTime = 0; // for the swing offset
elapsedMillis snare_env_ms;
int randSeedPool[1024];

// == Global Roller vars ==
// one global dataset for a mono roller; set by the play funcs, and executed by the main loop()
int rollCount = 0; // count up until subBeat
int rollDict[] = {4, 1, 80, 5, 4}; // one glob dict with {countDown, voice, vel, crescendo, numRollz};
const unsigned int subBeat = 12; // quantization step below the 1/16th. Cld be 60.... calc.' in µs.
const byte numSubdivisions = 5; // how many nTuples we can choose from.
byte rollChoices[numSubdivisions] =  {2, 3, 4, 6, 12}; // which subdivs are possible for rolls?
//byte rollGaps[numSubdivisions] =  {6, 4, 3, 2, 1};



void setup() {
  // init a big pool with rand nrs.
  randomSeed(1974);
  for (int i = 0; i < 1024; i++) {
    randSeedPool[i] = random(2147483647); // this shd be the long's range..
  }

  AudioMemory(20);
  audioShield.enable();
  audioShield.volume(_amp.outVal / 100.);

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
  Serial.println("AI\nPROBABALLISTIQ DRUMMER 1 \nHH 2021 hannes@airborneinstruments.eu\nwelcome! \n\n");
}

void loop() {
  usbMIDI.read();

  // end of bar
  if (seqStep >= int(_numSteps.outVal) ) {

    seqStep = 0;
    barCount++;
    digitalWrite(ledPin, 1);

    //    randomSeed( analogRead(1) );
    if (_free.outVal == 0) {
      randomSeed( randSeedPool[barCount % 1024] );
    }
    //     _thresh.outVal = float(random(1, 99)) / 100.0; // automatic threshold varying
    // every 8 bars
    if ( barCount % 8 == random(6, 8)) {
      fillSeed = random(100);
    } else {
      fillSeed = 0;
    };
    if (_free.outVal == 0) {
      randomSeed( _varSeed.outVal + fillSeed );
    }
  }

  //    if (snare_env_ms > 60) {    envelope1.noteOff();    }

  // roller in da main time loop.
  if (roller.hasPassed(usPerSubBeat), true) {
    rollFunc();
    //    rollCount++;
    rollCount = (rollCount + 1) % (16 * subBeat);
    //    Serial.println("\t rollCount: " + String(rollCount));

  }


  if (metro.hasPassed(msPerBeat)) {
    metro.restart();

    // Play Pattern
    if (seqPlay) {
      if (seqStep % 2 == 1) {
        delayTime = (msPerBeat * _swing.outVal * 0.01) + 1;
      } else {
        delayTime = 1;
        digitalWrite(ledPin, !digitalRead(ledPin));
      }

      //      Serial.println(seqStep);
      swinger.trigger(delayTime * 1000); // trigger the callback func playMyNote

      snare_env_ms = 0; // use for midi noteOFF ?
    }

    seqStep++;
  }

}
