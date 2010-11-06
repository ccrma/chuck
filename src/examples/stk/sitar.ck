// patch
Sitar sit => PRCRev r => dac;
.05 => r.mix;

// time loop
while( true )
{
    // freq
    Std.rand2( 0, 11 ) => float winner;
    Std.mtof( 57 + Std.rand2(0,3) * 12 + winner ) => sit.freq;

    // pluck!
    Std.rand2f( 0.4, 0.9 ) => sit.noteOn;

    // advance time
    if ( Std.randf() > 0.0 ) {
        .5::second => now;
    } else { 
        0.25::second => now;
    }
}
