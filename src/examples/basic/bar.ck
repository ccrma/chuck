// point of attack for random otf demo

SinOsc s => dac;
.2 => s.gain;

// an array: add stuff
[ 0 ] @=> int hi[];

while( true )
{
    // change parameters here
    Std.mtof( 45 + Math.random2(0,0) * 12 +
        hi[Math.random2(0,hi.size()-1)] ) => s.freq;

    // different rate
    200::ms => now;
}
