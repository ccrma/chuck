// tuned plucked string filter
// Ge Wang (gewang@cs.princeton.edu)

// feedforward
Noise imp => OneZero lowpass => PoleZero allpass => dac;
// feedback
allpass => Delay delay => lowpass;

// our radius
.99999 => float R;
// finding our (integer) delay order
Std.mtof( 80.75 ) => setFreq => float L;
// set delay
L::samp => delay.delay;
// set dissipation factor
Math.pow( R, L ) => delay.gain;
// place zero
-1 => lowpass.zero;

// fire excitation
1 => imp.gain;
// for one delay round trip
L::samp => now;
// done
0 => imp.gain;

// advance time
(Math.log(.0001) / Math.log(R))::samp => now;

// set (fundamental) freq
fun int setFreq( float freq )
{
    // sample rate
    second / samp => float SR;
    // omega
    2 * pi * freq / SR => float omega;
    // figure total delay needed
    SR / freq - .5 => float D;
    // the integer part
    D $ int => int Di;
    // the fraction
    D - Di => float Df;
    // set allpass using fractional and fundamental
    polePos( Df, omega ) => allpass.allpass;

    // return integer portion
    return Di;
}

// find pole location from delay and omega
fun float polePos( float D, float omega )
{
    // here it is (a la Jaffe & Smith)
    return Math.sin( (1-D) * omega / 2 ) / 
           Math.sin( (1+D) * omega / 2 );
}
