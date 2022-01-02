//-----------------------------------------------------------------------------
// name: readme-LiSa1.ck
// desc: Live sampling utilities for ChucK
//
// the LiSa ugens allow realtime recording of audio to a buffer for
// various kinds of manipulation. Below is a simple example
// demonstrating the basic functionality of LiSa.
//
// See readme-LiSa2.ck for a command summary and instructions for
// doing multiple voice playback.
//
// author: Dan Trueman, 2007
//-----------------------------------------------------------------------------

// signal chain; record a sine wave, play it back
SinOsc s => LiSa saveme => dac;
// monitor the input
s => dac;

// alloc memory; required
6::second => saveme.duration;
// freq
440 => s.freq;
// gain
0.25 => s.gain;

// start recording input
saveme.record(1);
// let time pass for 2 seconds
.5::second => now;
// stop recording
saveme.record(0); 

// disconnect; stop monitoring input
s =< dac;

// start playing what was just recorded, with panning...
// set playback rate
saveme.rate(1.5);
// set pan (0 is hard left, 1 is hard right)
saveme.pan(0.0);
// start playing, with a ramp up
// (use saveme.play(1) to start playing without ramp)
saveme.rampUp(500::ms); 

// hang for a bit
500::ms => now;

// rampdown
// (use saveme.play(0) to stop playing without ramp)
saveme.rampDown( 500::ms );

// wait for ramp to finish
600::ms => now;

// bye bye
