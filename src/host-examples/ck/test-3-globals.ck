// ChucK global event to wait on
global Event the_nextNote;
// ChucK global int to get pitch index
global float the_frequency;

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
    // copy to global
    s.freq() => the_frequency;

    // harmonics
    Math.random2( 1, 5 ) => s.harmonics;

    // key on
    e.keyOn();
    // wait on event
    the_nextNote => now;
    // key off
    e.keyOff();
    // advance time for release
    5::ms => now;
}
