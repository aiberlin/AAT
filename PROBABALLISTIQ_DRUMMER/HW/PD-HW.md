# Probaballistiq_Drummer - HW

hannes@airborneinstruments.eu - Spring 2021


## SW

All made in Teensyduino; Audio with the Teensy Audio design tool. 

- core: internal probaballistiq pattern generator
- internal, sample based 4 track audio seq: {kick, snr, clap, hats}. 
- USB MIDI note out
- USB MIDI cc in for all params
- ToDo later
	- test USB MIDI host for nanoKontrol etc faderboxes.
	- MIDI clock sync in/out 
	- DIN MIDI I/O
	- 4 track cv out


### Library dependencies:

##### Timing
- include "TeensyTimerTool.h" // https://github.com/luni64/TeensyTimerTool
- include <Chrono.h> // http://github.com/SofaPirate/Chrono

##### HW I/F

- include <Bounce.h>
- include <Encoder.h>
- https://github.com/tttapa/Control-Surface // for analogin filtering
- // don't ! include MIDIController // https://github.com/joshnishikawa/MIDIcontroller
- https://github.com/gavinlyonsrepo/TM1638plus // LED/Button I/F
	- We use "Model 1" as defined in TM1638plus.h. The library support ASCII, text, Hex and allows for setting individual segments, and the decimal point of segment. The TM1638plus_TEST.ino contains a set of tests demonstrating library functions. For more information see the commented headers in header file.
	- User may have to debounce buttons depending on application. > Teensy Bounce Class
	- String conversion using snprintf:
	   - printf ("Preceding with blanks: %10d \n", 1977);
	   - printf ("Preceding with blanks: %4d \n", 127);
	   https://www.cplusplus.com/reference/cstdio/snprintf/
	   https://www.cplusplus.com/reference/cstdio/printf/
	   - https://www.nutsvolts.com/uploads/wygwam/NV_0501_Marston_Figure02.jpg



##### Audio
- // Audio libs automatically included by Teensy Audio Design tool

=============

## HW 

### components:
- Teensy 4.0 for devel, [Teensy 3.2. f productuion]
- Teensy Audio shield
- Teensy 4 MQA as in <https://github.com/TeensyUser/doc/wiki/Audio-Example-I2S-without-AudioShield>
- TM1638 Model 1 Input/Display device
- 3.3 <> 5 V Level shifter for display
- 2 pots
- 1 Encoder

- Jack out, 
- Power Supply: ?? 
	- USB yes!
	- guit style 9V inv
	- 2 x AA Batt
	- LiPo Cell



=============


### Pinout:

#### Audio shield

	Audio Data T4: 
	7, 8, 20, 21, 23

	Audio Data T3: 
	9, 11, 13, 22, 23

	Audio Control T3/T4: 
	18, 19; shareable with SDA, SCL (other I2C chips)



#### TM1638

 3 wire serial interface.
Connections to MCU:

    GPIO = STB = Strobe
    GPIO = CLK = Clock
    GPIO = DIO = Data input / output
    GND
    VCC 5V.

This device is 5V if using 3.3V MCU, level shift.
https://github.com/gavinlyonsrepo/TM1638plus


#### Analox:
	3 digi pins for Encoder
	2 Analog pins for Pots


=========

= totalling to **15 GPIO** pins

=========

### encoder:  names
  -- SEED (default)
  -- ROLL
  -- VOL
  -- STEP
  -- BPM
  -- SWNG
  -- SKIP
  -- PLAY

  midi_type* midimap[] = { &_amp, &_BPM, &_numSteps, &_thresh, &_probDev, &_varSeed, &_seqPlay, &_swing, &_rollProb, &_skipTo };



===================
## varia: 

https://synthcube.com/cart/index.php?route=product/product&path=99_68&product_id=465
https://www.pjrc.com/breakout-board-for-teensy-4-0/