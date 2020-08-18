// STK ModalBar

// patch
Moog moog => dac;

// scale
[0, 2, 4, 7, 8, 11] @=> int scale[];

// infinite time loop
while( true )
{
    // ding!
    Math.random2f( 0, 1 ) => moog.filterQ;
    Math.random2f( 0, 1 ) => moog.filterSweepRate;
    Math.random2f( 0, 12 ) => moog.lfoSpeed;
    Math.random2f( 0, 1 ) => moog.lfoDepth;
    Math.random2f( 0, 1 ) => moog.volume;

    // print
    <<< "---", "" >>>;
    <<< "filterQ", moog.filterQ() >>>;
    <<< "filter sweep rate:", moog.filterSweepRate() >>>;
    <<< "lfo speed:", moog.lfoSpeed() >>>;
    <<< "lfo depth:", moog.lfoDepth() >>>;
    <<< "volume:", moog.volume() >>>;

    // set freq
    scale[Math.random2(0,scale.size()-1)] => int winner;
    57 + Math.random2(0,2)*12 + winner => Std.mtof => moog.freq;

    // go
    .8 => moog.noteOn;

    // advance time
    .5::second => now;
}
