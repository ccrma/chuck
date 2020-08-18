//---------------|
// modal demo
// by: Ge Wang (gewang@cs.princeton.edu)
//     Perry R. Cook (prc@cs.princeton.edu)
//------------------|

// our patch
ModalBar modey => JCRev r => dac;

// set the gain
.95 => r.gain;
// set the reverb mix
.1 => r.mix;

// scale
[ 0, 2, 4, 7, 9, 11 ] @=> int scale[];

int which;
// our main loop
while( true )
{
    // cycle through
    ( which + 1 ) % 9 => which;
    which => modey.preset;

    // frequency...
    scale[Math.random2(0,scale.size()-1)] => int freq;
    Math.mtof( 33 + (Math.random2(0,3)*12) + freq ) => modey.freq;

    // velocity
    Math.random2f( 0.4, 0.8 ) => float v;

    <<< "playing:", which, "freq:", Std.ftom(modey.freq()) $ int, "vel:", v >>>;

    repeat( 4 )
    {
        // position
        Math.random2f( 0.2, 0.8 ) => float p => modey.strikePosition;
        // strike it!
        v => modey.strike;
        // print
        <<< "    strike position:", p >>>;
        // advance time
        250::ms => now;
    }
}
