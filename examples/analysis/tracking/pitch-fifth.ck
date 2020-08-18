//--
// name: pitch-fifth.ck
// desc: tough pitch tracking + perfect fifth
//
// author: Rebecca Fiebrink + Ge Wang
// date: 2007
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
2048 => fft.size;
// window
Windowing.hamming( fft.size() ) => fft.window;

// interpolate
float target_freq, curr_freq, target_gain, curr_gain;
spork ~ ramp_stuff();

// go for it
while( true )
{
    // take fft
    fft.upchuck() @=> UAnaBlob blob;
    
    // find peak
    0 => float max; float where;
    for( int i; i < blob.fvals().size()/8; i++ )
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
    (where / fft.size() * (second / samp)) * Math.pow(1.059,7) => target_freq;
    // set gain
    (max / .5) => target_gain;
    
    // hop
    (fft.size()/4)::samp => now;
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
