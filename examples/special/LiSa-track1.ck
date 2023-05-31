//--------------------------------------------------------------------
// name: LiSa-track1.ck
// desc: live sampling utilities for ChucK
//       demonstrate using track == 1 mode with LiSa
//
// author: Dan Trueman, 2007
//
// when track == 1, the input is used to control playback position
// input [0,1] will control playback position within loop marks values
// less than zero are multiplied by -1, so it is possible to use audio
// signals [-1, 1] to control playback position, as in waveshaping
//--------------------------------------------------------------------

// signal chain; record a sine wave, play it back
SinOsc s => LiSa loopme => dac;
// frequency
440 => s.freq;
// gain
0.25 => s.gain;

// alloc LiSa memory
6::second => loopme.duration;
1000::ms => loopme.loopEndRec;
1000::ms => loopme.loopEnd;

// set recording ramp time
loopme.recRamp(25::ms);

// start recording input
loopme.record(1);
// wait for a second
1000::ms => now;
// stop recording input
loopme.record(0);

// set track mode to 1, where the input chooses playback position
// note that you can still record this input
1 => loopme.track;
// disconnect sinosc as input
s =< loopme;
// use envelope to control playback position
Step i => Envelope e => loopme;
// set step hold value
1 => i.next;

// play it forward twice speed
500::ms => e.duration;
// open envelope
e.keyOn();
// play
loopme.play(1);
// advance time
500::ms => now;

// now backwards half speed
2000::ms => e.duration;
// close envelope
e.keyOff();
// advance time
2000::ms => now;
// stop playing
loopme.play(0);

// bye bye
