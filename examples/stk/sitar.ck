// patch
Sitar sit => PRCRev r => dac;
.05 => r.mix;

// time loop
while( true )
{
    // freq
    Math.random2( 0, 11 ) => float winner;
    Std.mtof( 57 + Math.random2(0,3) * 12 + winner ) => sit.freq;

    // pluck!
    Math.random2f( 0.4, 0.9 ) => sit.noteOn;

    // advance time
    // note: Math.randomf() returns value between 0 and 1
    if( Math.randomf() > .5 ) {
        .5::second => now;
    } else { 
        0.25::second => now;
    }
}
