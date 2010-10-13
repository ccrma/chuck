// gen17 chebyshev lookup table
//
// ported to ChucK from rtcmix by dan trueman
// see luke's page: http://www.music.columbia.edu/cmix/makegens.html#06
//
// like gen10, this is a series of weightings for successive partials
//   also http://www.music.columbia.edu/cmc/Rtcmix/docs/scorefile/gen17.html
//   and http://www.music.columbia.edu/cmc/Rtcmix/docs/scorefile/maketable.html (scroll down)
//   for nice documentation of this gen
// see also: http://math.fullerton.edu/mathews/n2003/ChebyshevPolyMod.html
// for info about chebyshev polynomials in general
//
// the main feature to be aware of is that these functions are primarily
// useful for waveshaping, so in this case we drive it with a 
// SinOsc instead of a Phasor. see, for example:
//   http://crca.ucsd.edu/~msp/techniques/v0.08/book-html/node74.html
//   http://en.wikipedia.org/wiki/Distortion_synthesis

// the patch
SinOsc drive => Gen17 g17 => dac;

// load up the coeffs
[1., 0.5, 0.25, 0.125, 0.06, 0.03, 0.015] => g17.coefs;
// some examples to try
// [1., 0.5, 0.3, 0.2] => g17.coefs;
// [1., 0.5, 0.25, 0.125, 0.06, 0.03, 0.015] => g17.coefs;
// [1., 0.] @=> g17.coefs; // a simple one

// make it quiet
0.1 => g17.gain;

// set frequency for reading through table
drive.freq( 440 );
// sounds better not going to extremes
drive.gain( 0.99 ); 

// create an envelope to scan through the table values
// so we can look at them as well as hear them
Envelope e => blackhole;
e.duration( 10000::ms );
0. => e.value;
e.keyOn();

// loop
while (true)
{
    // print
    <<< e.value(), g17.lookup(e.value()) >>>;
    
    // stop when envelope reaches 1
    if( e.value() == 1. ) break;

    // advance time
    10::ms => now;
}
