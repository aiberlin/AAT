



void playStep()
{
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
      playRoll(voice);
    }

    if (gate) {
      byte velo = random(80, 127);
      playMyNote(voice, velo);
    }
  }
}


void playRoll(byte voice) {
  // seend according to what?
  int numRollz = random(2, 7);
  int rollzTimeDiv = msPerBeat / numRollz;
  byte velo = random(80, 100); // starting vel
  byte crescendo = random(0, 10) - 5;

  Serial.println(
    "rolling! num: " + String(numRollz) +
    "\tTimeDiv: " + String(rollzTimeDiv) +
    "\t voice: " + String(voice) +
    "\t velo: " + String(velo)
  );

  roller.restart();
  //  for (byte cnt = 0; cnt < numRollz; cnt++) {
  for (numRollz; numRollz > 0; numRollz--) {
    if (roller.hasPassed(rollzTimeDiv)) {

      Serial.println(
        "*** cnt: " + String(numRollz) +
        "\t voice: " + String(voice) +
        "\t velo: " + String(velo)
      );

      playMyNote(voice, velo);
      roller.restart();
    }
    velo = velo + crescendo;
  }
  roller.stop();
}



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

// @@@@@@@@@@@@@@@@@@@@ e.o. funcs @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@ e.o. funcs @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
