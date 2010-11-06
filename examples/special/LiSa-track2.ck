//-----------------------------------------------------------------------------
// name: LiSa-track2.ck
// desc: Live sampling utilities for ChucK
//
// author: Dan Trueman, 2007
//
// demonstrate using track=1 mode with LiSa
//
// when track == 1, the input is used to control playback position
// input [0,1] will control playback position within loop marks
// input values less than zero are multiplied by -1, so it is possible to use
// audio signals [-1, 1] to control playback position, as in waveshaping
//-----------------------------------------------------------------------------

// signal chain; record a sine wave, play it back
SinOsc s => LiSa loopme => dac;
440 => s.freq;
0.2 => s.gain;

// alloc memory
6::second => loopme.duration;
1000::ms => loopme.loopEndRec;
1000::ms => loopme.loopEnd;

// set recording ramp time
loopme.recRamp( 50::ms );

// start recording input
loopme.record( 1 );

// 1 sec later, stop recording
1000::ms => now;
loopme.record(0);

// set track mode to 1, where the input chooses playback position
1 => loopme.track;
0.5 => s.freq;
1. => s.gain; // need to reset gain if we want to get the full loop length
loopme.play( 1 );
loopme.gain( 0.1 );
5000::ms => now;
loopme.rampDown( 250::ms );
500::ms => now;

// bye bye
