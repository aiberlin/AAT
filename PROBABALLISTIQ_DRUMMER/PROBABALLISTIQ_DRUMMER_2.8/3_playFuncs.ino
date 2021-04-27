// note playing and roll funcs
// @@@@@@@@@@@@@@@@@@@@ funcs @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@ funcs @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

// just play a note on Audio & MIDI by voice=noteNum and velo
void playMyNote(byte voice, byte velo) {
  switch (voice) {
    case 0:
      mix1.gain(0, velo / 127.0 * 0.5);
      sound0.play(AudioSampleHihat);
      usbMIDI.sendNoteOn(notenums[0], velo, 0); // full vel for MIDI put - mix where u synthesize it!
      break;
    case 1:
      mix1.gain(1, velo / 127.0);
      usbMIDI.sendNoteOn(notenums[1], velo, 0);
      if (random(10) < 3) {
        sound1.play(AudioSampleDt01_clap_hipclap16);
      } else {
        sound1.play(AudioSampleDt01_clap_stereo99_16);
        // sound1.play(AudioSampleDt01_clap_harsh16);
      }
      break;
    case 2:
      mix1.gain(2, velo / 127.0);
      sound2.play(AudioSampleSnare);
      usbMIDI.sendNoteOn(notenums[2], velo, 0);
      break;
    case 3:
      mix1.gain(3, velo / 127.0);
      sound3.play(AudioSampleKick);
      usbMIDI.sendNoteOn(notenums[3], velo, 0);
      break;
  }
}



// just indexing into pattern and doing the random weight things.
void playStep() {
  for (byte voice = 0; voice < rows; voice++) {
    float val = steps[voice][seqStep % columns]; // modulo, in case more than 16 steps
    float thr = (1 - _thresh.outVal);
    float randThr = float(random(-1000, 1000)) / 1000.0 * _probDev.outVal;
    boolean gate = val >= (thr + randThr);

    // rollin'
    float rollVal = steps[voice][((seqStep + 1) % int(_numSteps.outVal)) % columns];
    boolean rollProb = random(100) < _rollProb.outVal;
    boolean rollGate = rollVal >= (thr + randThr) && rollProb;
    /*
        Serial.println(
          "rollVal: " +   String(rollVal) +
          "\t rollProb: " + String(rollProb) +
          "\t rollGate: " + String(rollGate) +
          "\t voice: " + String(voice)
          //      "val: " +   String(val) +
          //      "\t probDev: " + String(_probDev.outVal) +
          //      "\t thr: " + String(thr) +
          //      "\t randThr: " + String(randThr) +
          //      "\t gate: " + String(gate)
        );
    */
    if (rollGate) {
      scheduleRoll(voice);
    }

    if (gate) {
      byte velo = random(80, 127);
      playMyNote(voice, velo);
    }
  }
}



void scheduleRoll(byte voice) {
  // seend according to what?
  byte numRollz = rollChoices[random(numSubdivisions)];
  //  int rollzTimeDiv = msPerBeat / numRollz; // cld add a multiplier to go over 1/16 or 2/26 ...
  byte velo = random(80, 100); // starting vel
  byte crescendo = random(0, 10) - 5;

  rollDict[0] = numRollz;
  rollDict[1] = voice;
  rollDict[2] = velo;
  rollDict[3] = crescendo;
  rollDict[4] = numRollz; // keep in memory

  Serial.println(
    "rolling! num: " + String(numRollz) +
    //    "\tTimeDiv: " + String(rollzTimeDiv) +
    "\t voice: " + String(voice) +
    "\t velo: " + String(velo)
  );
}

// called all the time in Main loop() via Chrono::roller
void rollFunc() {
  if (rollDict[0] > 0) {
    // work thru the list
    if ( rollCount % rollDict[4] == 0) { // checks gaps in the subdivisions
      // rollDict = {numRollz, voice, velo, crescendo, numRollz};
      playMyNote(rollDict[1], rollDict[2]);  // play
      rollDict[2] = constrain(rollDict[2] + rollDict[3], 10, 127); // iterate vel/cresc
      rollDict[0]--; // decrement

      Serial.println(
        "*** cnt: " + String(rollDict[0]) +
        "\t voice: " + String(rollDict[1]) +
        "\t velo: " + String(rollDict[2]) 
        +         "\t rollCount: " + String(rollCount)

      );
    }

  }
}



// @@@@@@@@@@@@@@@@@@@@ e.o. funcs @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@ e.o. funcs @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
