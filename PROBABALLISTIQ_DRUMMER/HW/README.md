# Probaballistiq_Drummer - HW

### components:
- Teensy 4.0 or 3.2
- Teensy Audio shield
- Teensy 4 MQA as in <https://github.com/TeensyUser/doc/wiki/Audio-Example-I2S-without-AudioShield>
- TM1638 Model 1 Input/Display device
- 3.3 <> 5 V Level shifter for display, >= 3 chans
- 2 pots OR 1 joystick
- 1 Encoder
- Audio out Jack 
- Power Supply vie Teensy USB. 
Alt. options:
	- guit style 9V barrel jack, center neg.
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
	2 Analog pins for Pots OR 1 Joystick
	3 digital pins for Encoder RGB LED (optional)

= totalling to **18 GPIO** pins


**Pinout table Teensy 3.2:**

Pin Nr. | Func. 
--------|------- 
00		| reserved for Serial
01		| reserved for Serial
02		| T STB 
03		| T CLK 
04		| T DIO 
05		| Enc Dec
06		| Enc Inc
07		| LED R
08		| LED G
09		| Audio Shield
10		| LED B
11		| Audio Shield
13		| Audio Shield
14		| Pot/Joy 1
15		| Pot/Joy 2
17		| Enc Switch
19		| Audio Shield shareable
18		| Audio Shield shareable
22		| Audio Shield
23		| Audio Shield
A14		| optional LoFi Audio

**Pinout table Teensy 4:**

Pin Nr. | Func.  
--------|------- 
00		| reserved for Serial
01		| reserved for Serial
02		| T STB 
03		| T CLK 
04		| T DIO 
05		| Enc Dec
06		| Enc Inc
07		| Audio Shield
08		| Audio Shield
09		| LED R
10		| optional LoFi Audio R
11		| LED G
12		| optional LoFi Audio L
13		| reserved for LED internal
14		| Pot/Joy 1
15		| Pot/Joy 2
16		| LED B
17		| Enc Switch
19		| Audio Shield shareable
18		| Audio Shield shareable
20		| Audio Shield
21		| Audio Shield
23		| Audio Shield

![pinout pdf](Pinout.pdf)

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
