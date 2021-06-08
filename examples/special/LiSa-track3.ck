//--------------------------------------------------------------------
// name: LiSa-track3.ck
// desc: Live sampling utilities for ChucK
//       demonstrate using track == 1 mode with LiSa
//
// author: Dan Trueman, 2007
//
// when track == 1, the input is used to control playback position
// input [0,1] will control playback position within loop marks
// input values less than zero are multiplied by -1, so it is
// possible to use audio signals [-1, 1] to control playback position,
// as in waveshaping
//--------------------------------------------------------------------

// signal chain; record a sine wave, play it back
SinOsc s => LiSa loopme => dac;
// frequency
440 => s.freq;

// alloc LiSa memory
6::second => loopme.duration;
1000::ms => loopme.loopEndRec;
1000::ms => loopme.loopEnd;

// set recording ramp time
loopme.recRamp( 50::ms );
// retain some while loop recording
loopme.feedback( 0.99 ); 

// start recording input
loopme.record( 1 );
// 1 sec later, this time DON'T stop recording....
1000::ms => now;

// set track mode to 1, where the input chooses playback position
1 => loopme.track;
// this time don't change the freq; scan through zippy quick
loopme.play( 1 );
loopme.gain( 0.02 );

// advance time for 5 seconds
5000::ms => now;
// ramp down
loopme.rampDown( 250::ms );
// wait
500::ms => now;

// pretty farking scary
// bye bye
