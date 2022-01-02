// even more music for replicants

// patch
Wurley voc=> JCRev r => dac;

// initial settings
220.0 => voc.freq;
0.95 => voc.gain;
.8 => r.gain;
.1 => r.mix;

// scale
[ 0, 3, 7, 8, 11 ] @=> int scale[];

// our main time loop
while( true )
{
    // scale
    scale[Math.random2(0,scale.size()-1)] => int freq;
    Std.mtof( ( 45 + Math.random2(0,1) * 12 + freq ) ) => voc.freq;
    Math.random2f( 0.6, 0.8 ) => voc.noteOn;

    // note: Math.randomf() returns value between 0 and 1
    if( Math.randomf() > 0.9 )
    {
        // 1000::ms => now;
        repeat( 100 )
        {
            voc.freq() * 1.01 => voc.freq;
            10::ms => now;
        }
    }
    else if( Math.randomf() > .75 )
    {
        // 500::ms => now;
        repeat( 50 )
        {
            voc.freq() * .99 => voc.freq;
            10::ms => now;
        }
    }
    else if( Math.randomf() > .1 )
    {
        250::ms => now;

    }
    else
    {
        0 => int i;
        2 * Math.random2( 1, 3 ) => int pick;
        0 => int pick_dir;
        0.0 => float pluck;

        for( ; i < pick; i++ )
        {
            Math.random2f(.4,.6) + i*.035 => pluck;
            pluck + 0.03 * (i * pick_dir) => voc.noteOn;
            !pick_dir => pick_dir;
            250::ms => now;
        }
    }
}
