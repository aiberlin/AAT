

// @@@@@@@@@@@@@@@@@@@@ funcs @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@ funcs @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@ funcs @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

// http://www.sengpielaudio.com/calculator-bpmtempotime.htm
int convertBPMtoMS(int bpm) {
  msPerBeat = 60000 / bpm; // globalize
  usPerSubBeat = msPerBeat * 1000 / subBeat; // µs!
  return msPerBeat;
}
float midicps(float notenum) {
  return 440.0 * (pow (2, ((notenum - 69.0) / 12.0)));
}
//. unused
int _indexOf(byte inVal, byte arr[], byte len) { // ugly: works only for my 7 long array...
  int wantedpos = -1;
  //  int len = sizeof(arr) / sizeof(arr[0]);
  for (int i = 0; i < len; i++) {
    if (inVal == arr[i]) {
      wantedpos = i;
      break;
    }
  }
  return wantedpos;
}

// return index in struct array based upon ccnum input.
int _indexOfStruct(byte ccnum) {
  int wantedIndex = -1;
  //  for (auto struxt : midimap) // check test this better: https://www.geeksforgeeks.org/range-based-loop-c/
  for (int i = 0; i < numCCs; i++) {
    if (midimap[i]->ccnum == ccnum) {
      wantedIndex = i;
      break;
    }
  }
  return wantedIndex;
}

// return index in struct array based upon encoderIndex input.
int _encIndexOfStructDumm(byte addr) {
  int wantedIndex = -1;
  for (int i = 0; i < numCCs; i++) {
    if (midimap[i]->encoderIndex == addr) {
      wantedIndex = i;
      break;
    }
  }
  return wantedIndex;
}


// return index in struct array based upon encoderIndex input.
// https://en.cppreference.com/w/cpp/language/range-for
int _encIndexOfStruct(byte addr) {
  int wantedIndex = -1;
  int i = 0;
  //  for (int i = 0; i < numCCs; i++) {
  for (auto element : midimap) // access by value, the type of i is int
    if (element->encoderIndex == addr) {
      wantedIndex = i;
      break;
    }
  i++;

  return wantedIndex;
}


/*
   RealTimeSystem – process MIDI real time messages (MIDI system clock)
   Midiclock events are sent at a rate of 24 pulses per quarter note
*/
void RealTimeSystem(byte realtimebyte) {
  if (realtimebyte == 248)   // 248 = CLOCK
  {
    midi_clock_counter++;

    if (midi_clock_counter == midi_clockdivider)
    {
      midi_clock_counter = 0;
      seqStep++;
      //send stepIndex to MIDI
      usbMIDI.sendControlChange(30, seqStep, 1);
    }
  }
  if (realtimebyte == 250 || realtimebyte == 251)  // 250 = START and 251 = CONTINUE
  {
    midi_clock_counter = 0;
  }
  if (realtimebyte == 252)  // 252 = STOP
  {
  }
}



float fscale( float originalMin, float originalMax, float newBegin, float newEnd, float inputValue, float curve)
{
  float OriginalRange = 0;
  float NewRange = 0;
  float zeroRefCurVal = 0;
  float normalizedCurVal = 0;
  float rangedValue = 0;
  boolean invFlag = 0;
  // condition curve parameter
  // limit range
  if (curve > 10) curve = 10;
  if (curve < -10) curve = -10;

  curve = (curve * -.1) ; // - invert and scale - this seems more intuitive - postive numbers give more weight to high end on output
  curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function
  // Check for out of range inputValues
  if (inputValue < originalMin) {
    inputValue = originalMin;
  }
  if (inputValue > originalMax) {
    inputValue = originalMax;
  }
  OriginalRange = originalMax - originalMin;

  if (newEnd > newBegin) {
    NewRange = newEnd - newBegin;
  }
  else
  {
    NewRange = newBegin - newEnd;
    invFlag = 1;
  }
  zeroRefCurVal = inputValue - originalMin;
  normalizedCurVal  =  zeroRefCurVal / OriginalRange;   // normalize to 0 - 1 float
  if (originalMin > originalMax ) {
    return 0;
  }
  if (invFlag == 0) {
    rangedValue =  (pow(normalizedCurVal, curve) * NewRange) + newBegin;

  }
  else     // invert the ranges
  {
    rangedValue =  newBegin - (pow(normalizedCurVal, curve) * NewRange);
  }
  return rangedValue;
}


void skipTo_(byte step_) {
  seqStep = step_;
  metro.restart();
}

// @@@@@@@@@@@@@@@@@@@@ e.o. funcs @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@ e.o. funcs @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
