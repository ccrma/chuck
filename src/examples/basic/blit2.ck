// patch
Blit s => ADSR e => JCRev r => dac;
.5 => s.gain;
.05 => r.mix;

// set adsr
e.set( 5::ms, 3::ms, .5, 5::ms );

// an array
[ 0, 2, 4, 7, 9, 11 ] @=> int hi[];

// infinite time loop
while( true )
{
    // frequency
    Std.mtof( 33 + Math.random2(0,3) * 12 +
        hi[Math.random2(0,hi.size()-1)] ) => s.freq;

    // harmonics
    Math.random2( 1, 5 ) => s.harmonics;

    // key on
    e.keyOn();
    // advance time
    120::ms => now;
    // key off
    e.keyOff();
    // advance time
    5::ms => now;
}
