//---------------|
// shake-o-matic!
// by: Ge Wang (gewang@cs.princeton.edu)
//     Perry R. Cook (prc@cs.princeton.edu)
//------------------|

// our patch
Shakers shake => JCRev r => dac;
// set the gain
//.95 => r.gain;
// set the reverb mix
.025 => r.mix;

// our main loop
while( true )
{
    // frequency..
    // note: Math.randomf() returns value between 0 and 1
    if( Math.randomf() > 0.625 )
    {
        Math.random2( 0, 22 ) => shake.which;
        Std.mtof( Math.random2f( 0.0, 128.0 ) ) => shake.freq;
        Math.random2f( 0, 128 ) => shake.objects;
        <<< "instrument #:", shake.which(), shake.freq(), shake.objects() >>>;
    }

    // shake it!
    Math.random2f( 0.8, 1.3 ) => shake.noteOn;

    // note: Math.randomf() returns value between 0 and 1
    if( Math.randomf() > 0.9 )
    { 500::ms => now; }
    else if( Math.randomf() > .925 )
    { 250::ms => now; }
    else if( Math.randomf() > .05 )
    { .125::second => now; }
    else
    {
        1 => int i => int pick_dir;
        // how many times
        4 * Math.random2( 1, 5 ) => int pick;
        0.0 => float pluck;
        0.7 / pick => float inc;
        // time loop
        for( ; i < pick; i++ )
        {
            75::ms => now;
            Math.random2f(.2,.3) + i*inc => pluck;
            pluck + -.2 * pick_dir => shake.noteOn;
            // simulate pluck direction
            !pick_dir => pick_dir;
        }

        // let time pass for final shake
        75::ms => now;
    }
}
