// patch
Mandolin mand => dac;

// time loop
while( true )
{
    // pos
    std.rand2f ( 0.2, 0.8 ) => mand.pluckPos;
    // freq
    220.0 * math.pow( 1.059, (float)std.rand2(0,24) ) => mand.freq;
    // pluck!
    std.rand2f( 0.4, 0.9 ) => mand.pluck;

    // advance time
    if ( std.randf() > 0.0 ) {
        1::second => now;
    } else { 
        0.5::second => now;
    }
}
