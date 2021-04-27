# PROBABALLISTIQ_DRUMMER

musical Artificial Agents as Teensy audio devices

A generative HipHop drum machine with endless variations on funk grooves. 

Parameters controllable via MIDI; 
HW box in progress. 


------------------------

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
