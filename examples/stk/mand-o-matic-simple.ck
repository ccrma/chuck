//---------------|
// mand-o-matic! (simplified version)
// by: Ge Wang (ge@ccrma.stanford.edu)
//     Perry R. Cook (prc@cs.princeton.edu)
//------------------|

// our patch
Mandolin mand => JCRev r => dac;
// set the gain
.95 => r.gain;
// set the reverb mix
.05 => r.mix;

// scale
[ 0, 2, 4, 7, 9 ] @=> int scale[];

// our main loop
while( true )
{
    // position
    Math.random2f( 0.2, 0.8 ) => mand.pluckPos;
    // frequency...
    scale[Math.random2(0,scale.size()-1)] => int freq;
    45 + Math.random2(0,3)*12 + freq => Std.mtof => mand.freq;
    // pluck it!
    Math.random2f( 0.2, 0.9 ) => mand.pluck;
    
    // how much to wait
    if( Math.random2f(0,1) > .95 ) // long note
    { 500::ms => now; }
    else if( Math.random2f(0,1) > .95 ) // short
    { 250::ms => now; }
    else if( Math.random2f(0,1) > .05 ) // shorter
    { .125::second => now; }
    else // trill
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
            pluck + -.2 * pick_dir => mand.pluck;
            // simulate pluck direction
            !pick_dir => pick_dir;
        }
        // let time pass for final pluck
        75::ms => now;
    }
}
