// @@@@@@@@@@@@@@@@@@@@ funcs @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@ funcs @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@ funcs @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


void getControlChangeMsg(byte channel, byte control, byte value) {
  //    int idx = _indexOf(control, ccnumMap, numCCs); // clean
  int idx = _indexOfStruct(control); // clean
  if (idx >= 0) {
    if ( midimap[idx]->inVal != value ) { // no set if no change
      // take wal, scale it according to sctruct's specs, and push it onto struct.outVal
      midimap[idx]->inVal = value;
      midimap[idx]->outVal = fscale(0, 127, midimap[idx]->range[0], midimap[idx]->range[1], value, midimap[idx]->curve);
      // toDo: round to arbitrary grain midimap[idx]->stepSize;
      // midimap[idx]->outVal = map(value, 0, 127, midimap[idx]->range[0], midimap[idx]->range[1]);

      // MIDI map setters - do all to avoid comparing prev states..... actually, do look up prev states...
      audioShield.volume( _amp.outVal/ 100. );
      //  metro.interval(convertBPMtoMS(_BPM.outVal * 4));
      convertBPMtoMS(_BPM.outVal * 4); // globalized
      seqPlay = boolean(_seqPlay.outVal);
      if (_skipTo.outVal > 63) { // nice, but in an ugly place...
        skipTo_(4);
        _skipTo.outVal = 63;
      }
      if (_skipTo.outVal < 63) {
        skipTo_(4);
        _skipTo.outVal = 63;
      }

      // e.o. MIDI app
      Serial.println(
        "amp: " +   String(_amp.outVal) +
        "\tbpm: " + String(_BPM.outVal) +
        "\tsteps: " + String(_numSteps.outVal) +
        "\tthresh: " + String(_thresh.outVal) +
        "\tprobDev: " + String(_probDev.outVal) +
        "\tSeed: " + String(_varSeed.outVal) +
        "\tplay: " + String(_seqPlay.outVal) +
        "\tswing: " + String(_swing.outVal) +
        "\troll: " + String(_rollProb.outVal) +
        "\tskipTo: " + String(_skipTo.outVal)
      );
      // cache back:
      midimap[idx]->oldInVal = midimap[idx]->inVal;
      midimap[idx]->oldOutVal = midimap[idx]->outVal;
    }
  } else { // no matching CC map
    Serial.println("Error on ccnum " + String(control) + ". Not mapped?");
  }
}


// @@@@@@@@@@@@@@@@@@@@ e.o. funcs @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@ e.o. funcs @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
