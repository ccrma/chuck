//--------------------------------------------------------------------
// name: LiSa-track5.ck
// desc: live sampling utilities for ChucK
//       demonstrate using track == 2 mode with LiSa
//
// author: Dan Trueman, 2007
//--------------------------------------------------------------------

// patch
LiSa loopme => dac;
// use for loading audio file (also see example: LiSa-load.ck)
SndBuf buf;

// change this path to your own sample
me.dir() + "geetar.wav" => buf.read;
// set lisa buffer size to sample size
buf.samples()::samp => loopme.duration;
// transfer values from SndBuf to LiSa
for( 0 => int i; i < buf.samples(); i++ )
{ loopme.valueAt(buf.valueAt(i), i::samp); }

// set sync/track mode to 2, where the input chooses playback
// position interpreted as a time value
// again, negative values are multiplied internally by -1
2 => loopme.sync;

// use an oscillator to set playback position
SinOsc tracker => loopme;
// step signal as offset
Step off => loopme;

// tracker parameters; will determine how far into the buffer
// to go (gain), with what offset (off) and with what speed (freq)
0.05 => tracker.freq;
// we'll loop over half of the buffer
loopme.duration() / 4::samp => tracker.gain;
//starting at the midpoint
loopme.duration() / 2::samp => off.next;

// set gain
0.5 => loopme.gain;
// play it back
1 => loopme.play;

// time loop
while( true )
{
    // monitor where we're playing back from in the buffer
	<<< ( off.last() + tracker.last() ) / 44100 >>>;
	50::ms => now; 
}
