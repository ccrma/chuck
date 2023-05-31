//  "Greyhole" demo with decimation/down/upsampling
//  By Perry R. Cook, October 2012
// This demonstrates a couple of things
// 1) using FIR Sinc filters for bandlimiting and reconstruction
// 2) decimation, for doing signal processing at lower rates
//                or just for fun

int decFactor;  // can specify like:    chuck TestDownUpSample.ck:8
if (me.args() > 0) Std.atoi(me.arg(0)) => decFactor;
else 16 => decFactor;  // or it will default to what is here

Gain decimate;  // anything chucked to here will run at lower rate
adc => FIR lowpass => blackhole;  // initial band limiting if needed  
Step holdout => FIR lp2 => WvOut w => dac; // hold and reconstruct
"test.wav" => w.wavFilename;
0.1 => w.fileGain;

// lowpass is here to bandlimit before downsampling
1.0 => lowpass.gain; 128 => lowpass.order; decFactor => lowpass.sinc;
// lp2 is here to get rid of decimation critters after upsampling
// but doesn't have to be as high order (driven by a step)
1.0 => lp2.gain; 64 => lp2.order; decFactor => lp2.sinc;

spork ~ decimateBy(decFactor);

1 => int notDone;

SinOsc s => decimate;     // test to prove decimation works
10000.0 => s.freq;        // sine freq won't be this, it will
0.1 => s.gain;           // be thisFreq / decFactor  because it
                            // only runs at the lower rate!!

1.0 => lowpass.gain;   // this sets/mutes the microphone input

while(notDone)
{
    0.05 :: second => now;  // hang out here and do nothing
}

fun void decimateBy(int factor)  {
    while (notDone)  {          // only take every factor-th
	lowpass => decimate;    // sample from our input(s)
        decimate => blackhole;     // hook things up and let
        1 :: samp => now;        // one sample through
        decimate.last()=> holdout.next; // sample to output
	lowpass =< decimate;        // now disconnect everybody
        decimate =< blackhole;     // (probably too enthusiastically)
        (factor - 1) :: samp => now;   // and hang around between
    }       
}
