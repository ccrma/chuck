//-----------------------------------------------------------------------------
// name: LiSa
// desc: Live sampling utilities for ChucK
//
// author: Dan Trueman, 2007
//
// to run (in command line chuck):
//     %> chuck LiSa_SndBuf.ck
//
//-----------------------------------------------------------------------------

/* 

this example shows how to open a soundfile and use it in LiSa. someday LiSa
may be able to open soundfiles directly, but don't hold your breath. 

note that unlike SndBuf, LiSa wants a dur (not an int) to specify the index
of the sample location

*/

//-----------------------------------------------------------------------------

//ugens
SndBuf buf;
LiSa lisa => dac;

//change this path to your own sample
"/Users/dan/Files/Chuck/LiSa_examples/TomVega.wav" => buf.read;

//set lisa buffer size to sample size
buf.samples() * 1::samp => lisa.duration;

//transfer values from SndBuf to LiSa
for ( 0 => int i; i < buf.samples(); i++ ) {

	//args are sample value and sample index (dur must be integral in samples)
	lisa.valueAt(buf.valueAt(i), i::samp);
	
}

//party on...
1  => lisa.play;
2. => lisa.rate;

//look at some of the data
//returns value at given time, with linear interpolation
<<< Math.random2f( 0., lisa.duration() / 1::samp ) :: samp  => lisa.valueAt >>>;

//hang on until it's done...
lisa.duration() * 0.5 => now;
