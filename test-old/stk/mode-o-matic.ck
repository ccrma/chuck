//---------------|
// modal demo
// based off of mand-o-matic ( master plan ) 
// by : philipd 
// by: Ge Wang (gewang@cs.princeton.edu)
//     Perry R. Cook (prc@cs.princeton.edu)
//------------------|
// our patch

ModalBar modey => JCRev r => Echo a => Echo b => Echo c => dac;

// set the gain
.95 => r.gain;
// set the reverb mix
.2 => r.mix;
// set max delay for echo
1000::ms => a.max => b.max => c.max;
// set delay for echo
750::ms => a.delay => b.delay => c.delay;
// set the initial effect mix
0.0 => a.mix => b.mix => c.mix;

// shred to modulate the mix
fun void echo_shred( ) { 
    0.0 => float decider => float mix => float old => float inc;

    // time loop
    while( true )
    {
        std.rand2f(0.0,1.0) => decider;
        if( decider < .35 ) 0.0 => mix;
        else if( decider < .55 ) .08 => mix;
        else if( decider < .8 ) .5 => mix;
        else .15 => mix;

        // find the increment
        (mix-old)/1000.0 => inc; 1000 => int n;
        // time loop
        while( n-- )
        {
            // set the mix for a, b, c
            old + inc => old => a.mix => b.mix => c.mix;
            1::ms => now;
        }
        // remember the old
        mix => old;
        // let time pass until the next iteration
        std.rand2(2,6)::second => now;
    }
}

// let echo shred go
spork ~ echo_shred();

// our main loop
while( true )
{
    //presets 
    if ( std.randf() > 0.9 ) { 
        std.rand2 (0,8) => modey.preset;
    }
    // position
    std.rand2f( 0.2, 0.8 ) => modey.strikePosition;
    // frequency...
    2 * std.rand2( 0, 4 ) => int freq;
    if( freq == 6 ) 7 => freq; if( freq == 8 ) 9 => freq;
    110.0 * math.pow( 1.05946, (std.rand2(0,3)*12) +freq ) => modey.freq;
    // pluck it!
    std.rand2f( 0.2, 0.6 ) => modey.strike;

    if( std.randf() > 0.8 )
    { 500::ms => now; }
    else if( std.randf() > .85 )
    { 250::ms => now; }
    else if( std.randf() > -0.9 )
    { .125::second => now; }
    else
    {
        1 => int i => int pick_dir;
        // how many times
        4 * std.rand2( 1, 5 ) => int pick;
        0.0 => float pluck;
        0.65 / pick => float inc;
        // time loop
        for( ; i < pick; i++ )
        {
            75::ms => now;
            std.rand2f(.2,.3) + i*inc => pluck;
            pluck => modey.stickHardness;
            pluck + -.2 * pick_dir => modey.strike;
            // simulate pluck direction
            !pick_dir => pick_dir;
        }
        // let time pass for final pluck
        75::ms => now;
    }
}
