// STK ModalBar

// patch
Moog moog => dac;

// scale
[0, 2, 4, 7, 8, 11] @=> int scale[];

// infinite time loop
while( true )
{
    // ding!
    Std.rand2f( 0, 1 ) => moog.filterQ;
    Std.rand2f( 0, 1 ) => moog.filterSweepRate;
    Std.rand2f( 0, 12 ) => moog.lfoSpeed;
    Std.rand2f( 0, 1 ) => moog.lfoDepth;
    Std.rand2f( 0, 1 ) => moog.volume;

    // print
    <<< "---", "" >>>;
    <<< "filterQ", moog.filterQ() >>>;
    <<< "filter sweep rate:", moog.filterSweepRate() >>>;
    <<< "lfo speed:", moog.lfoSpeed() >>>;
    <<< "lfo depth:", moog.lfoDepth() >>>;
    <<< "volume:", moog.volume() >>>;

    // set freq
    scale[Std.rand2(0,scale.cap()-1)] => int winner;
    57 + Std.rand2(0,2)*12 + winner => Std.mtof => moog.freq;

    // go
    .8 => moog.noteOn;

    // advance time
    .5::second => now;
}
