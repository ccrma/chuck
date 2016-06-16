// banded waveguide sample

// the patch
BandedWG band => JCRev r => dac;

// presets
0.95 => band.gain;
1 => band.preset;
Std.mtof( ( 21 + Math.random2(0,3) * 12 + 7 ) ) => band.freq;
.8 => r.gain;
.1 => r.mix;

// scale
[ 0, 2, 4, 7, 9 ] @=> int scale[];

// our main time loop
while( true )
{
    Math.random2f( 0.1, 0.9 ) => band.bowRate;
    Math.random2f( 0.2, 0.35 ) => band.bowPressure;
    Math.random2f( 0.6, 0.8 ) => band.startBowing;

    // note: Math.randomf() returns value between 0 and 1
    if( Math.randomf() > 0.85 )
    { 1000::ms => now; }
    else if( Math.randomf() > .85 )
    { 500::ms => now; }
    else if( Math.randomf() > 0.6 )
    { .250::second => now; }
    else
    {
        0 => int i;
        4 * Math.random2( 1, 4 ) => int pick;
        0 => int pick_dir;
        0.0 => float pluck;
        Math.random2f( 50.0, 200.0 ) => float d;

        for( ; i < pick; 1 +=> i )
        {
            Math.random2f(.4,.6) + i*.35/pick => pluck;
            pluck + 0.1 * pick_dir => band.pluck;
            !pick_dir => pick_dir;
            d::ms => now;
        }
    }

    // note: Math.randomf() returns value bewteen 0 and 1
    if( Math.randomf() > 0.2 )
    { 
        1::second => now;
        0.001 => band.stopBowing;
        0.5::second *  Math.random2(1,3) => now;

        // scale
        scale[Math.random2(0, scale.size()-1)] => int freq;
        Std.mtof( 21 + Math.random2(0,5) * 12 + freq ) => band.freq;
        // note: Math.randomf() returns value between 0 and 1
        if( Math.randomf() > 0.85 ) 
            Math.random2(0,3) => band.preset;
    }
}
