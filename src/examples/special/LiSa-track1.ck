//-----------------------------------------------------------------------------
// name: LiSa-track1.ck
// desc: Live sampling utilities for ChucK
//
// author: Dan Trueman, 2007
//
// demonstrate using track=1 mode with LiSa
//
// when track == 1, the input is used to control playback position
// input [0,1] will control playback position within loop marks
// values less than zero are multiplied by -1, so it is possible to use
// audio signals [-1, 1] to control playback position, as in waveshaping
//-----------------------------------------------------------------------------

// signal chain; record a sine wave, play it back
SinOsc s => LiSa loopme => dac;
440. => s.freq;
0.2 => s.gain;

// alloc memory
6::second => loopme.duration;
1000::ms => loopme.loopEndRec;
1000::ms => loopme.loopEnd;

// set recording ramp time
loopme.recRamp(25::ms);

// start recording input
loopme.record(1);

// 1 sec later, stop recording
1000::ms => now;
loopme.record(0);

// set track mode to 1, where the input chooses playback position
// note that you can still record this input
1 => loopme.track;
s =< loopme; // disconnect sinosc as input
Step i => Envelope e => loopme; // use envelope to control playback position
1. => i.next;

// play it forward twice speed
500::ms => e.duration;
e.keyOn();
loopme.play(1);
500::ms => now;

// now backwards half speed
2000::ms => e.duration;
e.keyOff();
2000::ms => now;
loopme.play(0);

// bye bye
