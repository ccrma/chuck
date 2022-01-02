//-----------------------------------------------------------------------------
// name: LiSa-simplelooping.ck
// desc: Live sampling utilities for ChucK
//
// author: Dan Trueman, 2007
//
// Another simple example of LiSa, demonstrating dopey looping...
//-----------------------------------------------------------------------------

// signal chain; record a sine wave, play it back
SinOsc s => LiSa loopme => dac;
// set frequency
440. => s.freq;
// set gain
0.25 => s.gain;

// alloc memory
1::second => loopme.duration;
// ramp time of 200 ms
loopme.recRamp( 200::ms );

// start recording input
loopme.record(1);
// 1 second later
1000::ms => now;
// stop recording;
loopme.record(0);

// next, start playing what was just recorded...

// set playback rate
loopme.rate(1.5);
// set loop to true
loopme.loop(1);
// enable bi-directional looping
loopme.bi(1);
// play (voice 0)
loopme.play(1);

// keep alive loop
while( true )
{ 500::ms => now; }

// bye bye
