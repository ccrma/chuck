//--
// name: pitch-track.ck
// desc: rough pitch tracking
//
// author: Rebecca Fiebrink + Ge Wang
//   date: 2007
//--


// analysis
adc => PoleZero dcblock => FFT fft => blackhole;
// synthesis
SinOsc s => JCRev r => dac;

// set reverb mix
.05 => r.mix;
// set to block DC
.99 => dcblock.blockZero;
// set FFT params
1024 => fft.size;
// window
Windowing.hamming( fft.size() ) => fft.window;

// to hold result
UAnaBlob blob;
// find sample rate
second / samp => float srate;

// interpolate
float target_freq, curr_freq, target_gain, curr_gain;
spork ~ ramp_stuff();

// go for it
while( true )
{
    // take fft
    fft.upchuck() @=> blob;
    
    // find peak
    0 => float max; int where;
    for( int i; i < blob.fvals().size(); i++ )
    {
        // compare
        if( blob.fvals()[i] > max )
        {
            // save
            blob.fvals()[i] => max;
            i => where;
        }
    }
    
    // set freq
    (where $ float) / fft.size() * srate => target_freq;
    // set gain
    (max / .8) => target_gain;
    
    // hop
    (fft.size()/2)::samp => now;
}


// interpolation
fun void ramp_stuff()
{
    // mysterious 'slew'
    0.025 => float slew;
    
    // infinite time loop
    while( true )
    {
        (target_freq - curr_freq) * 5 * slew + curr_freq => curr_freq => s.freq;
        (target_gain - curr_gain) * slew + curr_gain => curr_gain => s.gain;
        0.0025::second => now;
    }
}
