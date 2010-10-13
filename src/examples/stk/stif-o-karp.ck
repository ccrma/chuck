//---------------|
// karp-o-matic!
// by: Ge Wang (gewang@cs.princeton.edu)
//     Perry R. Cook (prc@cs.princeton.edu)
//------------------|

// our patch
StifKarp karp => JCRev r => Echo a => Echo b => Echo c => dac;
// set the gain
.95 => r.gain;
// set the reverb mix
.1 => r.mix;
// set max delay for echo
1000::ms => a.max => b.max => c.max;
// set delay for echo
750::ms => a.delay => b.delay => c.delay;
// set the initial effect mix
0.0 => a.mix => b.mix => c.mix;

// shred to modulate the mix
fun void echo_Shred( )
{
    0.0 => float decider => float mix => float old => float inc;

    // time loop
    while( true )
    {
        Std.rand2f(0.0,1.0) => decider;
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
        Std.rand2(2,6)::second => now;
    }
}

// let echo shred go
spork ~ echo_Shred();

// scale
[ 0, 2, 4, 7, 9 ] @=> int scale[];

// our main loop
while( true )
{
    // position
    Std.rand2f( 0.2, 0.8 ) => karp.pickupPosition;
    // frequency...
    scale[Std.rand2(0,scale.cap()-1)] => int freq;
    220.0 * Math.pow( 1.05946, (Std.rand2(0,2)*12)
                      +freq ) => karp.freq;
    // pluck it!
    0.0 => karp.stretch;
    Std.rand2f( 0.2, 0.9 ) => karp.pluck;

    if( Std.randf() > 0.8 )
    { 500::ms => now; }
    else if( Std.randf() > .85 )
    { 250::ms => now; }
    else if( Std.randf() > -0.9 )
    { .125::second => now; }
    else
    {
        1 => int i => int pick_dir;
        // how many times
        4 * Std.rand2( 1, 5 ) => int pick;
        0.0 => float pluck;
        0.7 / pick => float inc;
        // time loop
        for( ; i < pick; i++ )
        {
            75::ms => now;
            Std.rand2f(.2,.3) + i*inc => pluck;
            i * 0.025 => karp.stretch;
            pluck + -.2 * pick_dir => karp.pluck;
            // simulate pluck direction
            !pick_dir => pick_dir;
        }
        // let time pass for final pluck
        75::ms => now;
    }
}
