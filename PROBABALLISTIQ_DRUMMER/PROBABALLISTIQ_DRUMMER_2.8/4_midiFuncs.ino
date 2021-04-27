// all MIDI Funcs / utils
// @@@@@@@@@@@@@@@@@@@@ funcs @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@ funcs @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void softTakeover(byte idx, float normdVal) {
  if ( midimap[idx]->inVal != normdVal ) { // no set if no change
    float absdif = abs(normdVal - midimap[idx]->inVal) ;
    if ( absdif < softTakeOverTolerance ) { // soft takeover - 5 MIDI Vals tolerance
      midimap[idx]->inVal = normdVal;
      midimap[idx]->outVal = fscale(0, 1.0, midimap[idx]->range[0], midimap[idx]->range[1], midimap[idx]->inVal, midimap[idx]->curve);
      // toDo: round to arbitrary grain midimap[idx]->stepSize;

      //        if ( midimap[idx]->oldOutVal != midimap[idx]->outVal ) { // redundant
      setFunc(idx);
      //        }

      Serial.println(
        "MIDI - " + midimap[idx]->ctlName + " \t" + String(midimap[idx]->outVal)
      );

      // e.o. MIDI app
      /*  Serial.println(
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
        );*/
      // cache back:
      midimap[idx]->oldInVal = midimap[idx]->inVal;  
      midimap[idx]->oldOutVal = midimap[idx]->outVal;
    }
  }
}

void getControlChangeMsg(byte channel, byte control, byte value) {
  //    int idx = _indexOf(control, ccnumMap, numCCs); // clean
  int idx = _indexOfStruct(control); // clean lookup
  if (idx >= 0) {
    softTakeover(idx, value / 127.0);
  } else { // no matching CC map
    Serial.println("Error on ccnum " + String(control) + ". Not mapped?");
  }
}

// only a FEW param changes need an action; most are global params
void setFunc(byte idx) { // index is array index
  switch (idx) {
    case 0:
      audioShield.volume( _amp.outVal / 100.0 );
      break;
    case 1:
      convertBPMtoMS(_BPM.outVal * 4); // globalized
      break;
    case 9:
      if (_skipTo.outVal > 1) { // nice, but maybe ugly for MIDI/enc
        skipTo_(4);
        _skipTo.outVal = 1;
      }
      if (_skipTo.outVal < 1) {
        skipTo_(0);
        _skipTo.outVal = 1;
      }
      break;
      //  default:
      //  if nothing else matches, do the default
      //  break;
  }
  /*
    // MIDI map setters - do all to avoid comparing prev states..... actually, do look up prev states...
    audioShield.volume( _amp.outVal );
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
  */
}
// @@@@@@@@@@@@@@@@@@@@ e.o. funcs @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@ e.o. funcs @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
