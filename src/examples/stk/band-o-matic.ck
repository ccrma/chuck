// banded waveguide sample

// the patch
BandedWG band => JCRev r => dac;

// presets
0.95 => band.gain;
1 => band.preset;
Std.mtof( ( 21 + Std.rand2(0,3) * 12 + 7 ) ) => band.freq;
.8 => r.gain;
.1 => r.mix;

// scale
[ 0, 2, 4, 7, 9 ] @=> int scale[];

// our main time loop
while( true )
{    
    Std.rand2f( 0.1, 0.9 ) => band.bowRate;
    Std.rand2f( 0.2, 0.35 ) => band.bowPressure;
    Std.rand2f( 0.6, 0.8 ) => band.startBowing;

    if( Std.randf() > 0.7 )
    { 1000::ms => now; }
    else if( Std.randf() > .7 )
    { 500::ms => now; }
    else if( Std.randf() > 0.2 )
    { .250::second => now; }
    else
    {
        0 => int i;
        4 * Std.rand2( 1, 4 ) => int pick;
        0 => int pick_dir;
        0.0 => float pluck;
        Std.rand2f( 50.0, 200.0 ) => float d;

	for( ; i < pick; 1 +=> i )
        {
            Std.rand2f(.4,.6) + i*.35/pick => pluck;
            pluck + 0.1 * pick_dir => band.pluck;
            !pick_dir => pick_dir;
            d::ms => now;
        }
    }

    if ( Std.randf() > 0.2 )
    { 
        1::second => now;
        0.001 => band.stopBowing;
        0.5::second *  Std.rand2(1,3) => now;

        // scale
        scale[Std.rand2(0, scale.cap()-1)] => int freq;
        Std.mtof( 21 + Std.rand2(0,5) * 12 + freq ) => band.freq;
        if ( Std.randf() > 0.7 ) 
            Std.rand2(0,3) => band.preset;
    }
}
