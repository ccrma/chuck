// gen9 inharmonic sinusoidal lookup table
// ported to ChucK from rtcmix by dan trueman
// see luke's page: http://www.music.columbia.edu/cmix/makegens.html

// patch
Phasor drive => Gen9 g9 => dac;

// can also lookup table values directly:
//     g9.lookup(index);
//     with index:[0,1]

// load up the coeffs
// these are in triplets:
//     harmonic partial ratio, amplitude, phase
//     see the RTcmix page: 
//     http://www.music.columbia.edu/cmc/Rtcmix/docs/scorefile/gen9.html
//     for nice documentation of this gen
[1., 1., 0.,   2, 0.5, 90.,   3., 0.2, 180.] => g9.coefs;
// [1., 1., 0.] => g9.coefs; // a simple one

// make it quiet
0.1 => g9.gain;

// set frequency for reading through table
drive.freq( 440. );

// create an envelope to scan through the table values
// so we can look at them as well as hear them
Envelope e => blackhole;
e.duration(10000::ms);
0. => e.value;
e.keyOn();

// yay
while (true)
{
    // print
    <<< e.value(), g9.lookup(e.value()) >>>;

    // end
    if( e.value() == 1. ) break;

    // advance time
    10::ms => now;
}
