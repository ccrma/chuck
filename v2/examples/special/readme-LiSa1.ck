//-----------------------------------------------------------------------------
// name: readme-LiSa1.ck
// desc: Live sampling utilities for ChucK
//
// author: Dan Trueman, 2007
//
// the LiSa ugens allow realtime recording of audio to a buffer for various
// kinds of manipulation. 
//
// Below is a simple example demonstrating the basic functionality of
// LiSa.
//
// See readme-LiSa2.ck for a command summary and instructions for doing
// multiple voice playback.
//-----------------------------------------------------------------------------

//signal chain; record a sine wave, play it back
SinOsc s => LiSa saveme => dac;

s => dac; //monitor the input
440. => s.freq;
0.2 => s.gain;

//alloc memory; required
60::second => saveme.duration;

//start recording input
saveme.record(1);

//1 sec later, start playing what was just recorded, with panning
2000::ms => now;

s =< dac; //stop monitoring input
saveme.record(0); //stop recording
saveme.rate(1.5); //set playback rate
saveme.pan(0.); //set pan (0 is hard left, 1 is hard right)
saveme.rampUp(500::ms); //start playing, with a ramp up
//use saveme.play(1) to start playing without ramp

//hang for a bit
500::ms => now;

//rampdown
saveme.rampDown(500::ms);
//use saveme.play(0) to stop playing without ramp

//wait for ramp to finish
600::ms => now;

//bye bye
//-----------------------------------------------------------------------------

