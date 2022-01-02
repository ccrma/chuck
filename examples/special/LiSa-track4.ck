//--------------------------------------------------------------------
// name: LiSa-track4.ck
// desc: live sampling utilities for ChucK
//       demonstrate using track == 1 mode with LiSa
//
// author: Dan Trueman, 2007
//--------------------------------------------------------------------

// patch
LiSa loopme => dac;
// use for loading audio file (also see example: LiSa-load.ck)
SndBuf buf;

// change this to your own audio file
me.dir() + "geetar.wav" => buf.read;
// set lisa buffer size to sample size
buf.samples()::samp => loopme.duration;
// transfer values from SndBuf to LiSa
for( 0 => int i; i < buf.samples(); i++ )
{ loopme.valueAt(buf.valueAt(i), i::samp); }

// set sync/track mode to 1, where the input chooses playback position
1 => loopme.sync;

// use an oscillator to set playback position
SinOsc tracker => loopme;
// step signal as offset
Step off => loopme;

// tracker parameters; will determine how far into the buffer
// to go (gain), with what offset (off) and with what speed (freq)
0.05 => tracker.freq;
0.25 => tracker.gain;
.5 => off.next;

// set gain
0.5 => loopme.gain;
// play it back
1 => loopme.play;

// infinite time-loop
while ( true ) { 1::second => now; }
