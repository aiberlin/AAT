# Probaballistiq_Drummer - HW

### components:
- Teensy 4.0 or 3.2
- Teensy Audio shield
- Teensy 4 MQA as in <https://github.com/TeensyUser/doc/wiki/Audio-Example-I2S-without-AudioShield>
- TM1638 Model 1 Input/Display device
- 3.3 <> 5 V Level shifter for display
- 2 pots
- 1 Encoder

- Jack out, 
- Power Supply - Options:
	- USB yes!
	- guit style 9V inv
	- 2 x AA Batt
	- LiPo Cell


------------


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


#### Analog:
	3 digi pins for Encoder
	2 Analog pins for Pots / or 1 Joystick


---------

= totalling to **15 GPIO** pins

---------

### encoder:  names
  - SEED (default)
  - ROLL
  - VOL
  - STEP
  - BPM
  - SWNG
  - SKIP
  - PLAY

  	midi_type* midimap[] = { &_amp, &_BPM, &_numSteps, &_thresh, &_probDev, &_varSeed, &_seqPlay, &_swing, &_rollProb, &_skipTo };



-----------------
## unrelated varia: 

https://synthcube.com/cart/index.php?route=product/product&path=99_68&product_id=465

https://www.pjrc.com/breakout-board-for-teensy-4-0/
