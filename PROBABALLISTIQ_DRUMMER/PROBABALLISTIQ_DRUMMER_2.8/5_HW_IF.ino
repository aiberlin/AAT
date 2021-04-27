// HW interface on the Drummer Box

/*

  # ANALOG IN
  this for reading the analox, filtered:
  https://tttapa.github.io/Control-Surface-doc/Doxygen/dc/d58/FilteredAnalog_8ino-example.html

  # ENCODER
  https://www.pjrc.com/teensy/td_libs_Encoder.html

  OR... from same tttapa lib as analog in (ControlSurface) // seems to be using the Teensy one anyways...
  https://tttapa.github.io/Control-Surface-doc/Doxygen/da/dbf/classGenericEncoderSelector.html

  https://tttapa.github.io/Control-Surface-doc/Doxygen/d2/d1f/RotaryEncoder_8ino-example.html

  // misuse, but a way to get position
  https://tttapa.github.io/Control-Surface-doc/Doxygen/d1/da6/Encoder-Selector-Bank_8ino-example.html


  # TM1638 Input/Display device:
  https://github.com/gavinlyonsrepo/TM1638plus


  algorithm:
  - buttons set encoderIndex
  - clock keeps it for 3 secs after release. So, that's a special kinda debounce on encoderIndex=0.
  - doEncoder brings inc/dec and sets the values
  - display String goes to midimap[idx]->ctlName ++ snprintf (“%4d”, midimap[idx]->outVal);  also 3 secs longer after release

  - when to update display?
  -- whenever a Button is pressed or the Encoder moves, {or MIDI input, hehe}
  -- displayTimer.restart(); is re-triggered by any Button and Encoder
  -- when finished, sets encoderIndex to 0 and updateDisplay();

  - model contains:
  -- String writeDisplayString[8]
  -- byte cachedButton // the one that only the timer resets to 0.
  -- at least 1 byte for the LEDs
  -- or 2 bytes for blinking, and a byte with the PWM ratio value.

  - permanent Buttons reading action:
*/

// @@@@@@@@@@@@@@@@@@@@ funcs @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@ funcs @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

/*
// push String onto Display. triggered by Buttons and Encoders, later MIDI in
void writeDisplayString() {
  char valuesLower[5];
  // where do I generate the string? on push only? aka here?
  // I need to know the currently selected struct from the array, an read it's string and outVal
  byte idx = 0; // fix (globalize??)
  // https://www.cplusplus.com/reference/cstdio/sprintf/
  //  displayString = (midimap[idx]->ctlName)
  //                  + snprintf(valuesLower, 5, "%4d", midimap[idx]->outVal);  // Preceding with blanks: %4d
  displayString = (midimap[idx]->ctlName)
                  + String(sprintf(valuesLower, 5, "%4d", (midimap[idx]->outVal)));  // Preceding with blanks: %4d

  displayString.toCharArray(displayStringBuf, 8); // does it remove the null char at the end?
  tm.displayText(displayStringBuf); // needs Buf of Chars, gets the global one
}

void displayLEDs() {
  // beat indicator
  uint16_t ledString; // 2 bytes, higher byte only counts
  byte pos = 0;
  if (seqStep < 8) {
    pos = seqStep;
  } else {
    if (seqStep < 16) {
      pos = 8 - seqStep;
    } else {
      pos = seqStep - 16;
      // invert
      ledString = 0x0000 || (0x0100 << (pos)); // red dot moving
    }
    ledString = 0xFF00 && (0 << (pos + 8)); // black dot moving
  }
  tm.setLEDs(ledString);
  // // void TM1638plus::setLED(uint8_t position, uint8_t value) // cheaper to set all 8 separately?
}

// oy, they need debouncing, no? 10ms equality check manual debounce will do
void readButts() {
  buttByte = tm.readButtons();
  if (buttByte == checkButtByte) { // debouncery: must be same as last reading
    if (buttByte != oldButtByte) {
      if (buttByte != 0) {
        cachedButton = buttByte;
        globalButtIdx = _encIndexOfStruct(cachedButton); // easier if globalized
        writeDisplayString(); // then write string
        displayTimer.restart();
      }
      buttByte = oldButtByte; // copy only if changed
    }
  }
  checkButtByte = buttByte; // always copy
}
*/


void readAnalox() {
  if (threshPot.update()) {
    softTakeover(3, float(threshPot.getValue()) / 1203.0 ); // defined in 2_midiFuncs.ino
  }
  if (devPot.update()) {
    softTakeover(4, float(devPot.getValue()) / 1203.0 );
  }

}


// https://www.pjrc.com/teensy/td_libs_Encoder.html
void readEncoda() {
  int value = encoda.read();
  doEncoder(value, 1.0); // no multiplier for now.. multiplier cld be a mapping of enc time interval
  encoda.write(0);
//  writeDisplayString();
  if (cachedButton != 0) {
    displayTimer.restart(); // always restart display timer, unless 0
  }
}


// action when encoder triggers inc or dec.
// target is the byte address entered by the 8 buttons; value is encoder inc/dec
void doEncoder(char value, float multiplier) {

  // map target byte to idx
  int idx = globalButtIdx; // _encIndexOfStruct(target); // clean lookup, as for MIDI Func
  if (idx >= 0) {
    if ( value != 0 ) { // no set if input shd be 0

      float newInVal = constrain(
                         midimap[idx]->inVal + (value * multiplier / midimap[idx]->stepSize),
                         0.0, 1.0
                       );

      if ( midimap[idx]->inVal != newInVal ) {
        midimap[idx]->outVal = fscale(0, 1.0, midimap[idx]->range[0], midimap[idx]->range[1], midimap[idx]->inVal, midimap[idx]->curve);
        setFunc(idx); // defined in 2_midiFuncs.ino
        // cache back:
        midimap[idx]->inVal = newInVal;
        midimap[idx]->oldOutVal = midimap[idx]->outVal;

        Serial.println(
          "ENC - " + midimap[idx]->ctlName + " \t" + String(midimap[idx]->outVal)
        );

        /* Serial.println(
          "ENC amp: " +   String(_amp.outVal) +
          "\tbpm: " + String(_BPM.outVal) +
          "\tsteps: " + String(_numSteps.outVal) +
          "\tthresh: " + String(_thresh.outVal) +
          "\tprobDev: " + String(_probDev.outVal) +
          "\tSeed: " + String(_varSeed.outVal) +
          "\tplay: " + String(_seqPlay.outVal) +
          "\tswing: " + String(_swing.outVal) +
          "\troll: " + String(_rollProb.outVal) +
          "\tskipTo: " + String(_skipTo.outVal)
          );*/
      } else {
        Serial.println("Encoder " + midimap[idx]->ctlName + "reached min/max");
      }
    }
  } else { // no matching Encoder map
    Serial.println("Error on Enc target " + midimap[idx]->ctlName + ": Not mapped?");
  }
}

// @@@@@@@@@@@@@@@@@@@@ e.o. funcs @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@ e.o. funcs @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
