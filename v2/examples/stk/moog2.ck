// STK ModalBar

// patch
Moog moog => dac;

// scale
[0, 2, 4, 7, 8, 11] @=> int scale[];

// infinite time loop
while( true )
{
    // ding!
    Std.rand2f( 0, 128 ) => float filterQ;
    Std.rand2f( 0, 128 ) => float filterSweep;
    Std.rand2f( 0, 128 ) => float vol;
    Std.rand2f( 0, 128 ) => float vibratoFreq;
    Std.rand2f( 0, 128 ) => float vibratoGain;

    moog.controlChange( 2, filterQ);
    moog.controlChange( 4, filterSweep);
    moog.controlChange( 11, vibratoFreq);
    moog.controlChange( 1, vibratoGain);
    moog.controlChange( 128, vol);

    <<< "---", "" >>>;
    <<< "filterQ", moog.filterQ() >>>;
    <<< "filter sweep rate:", moog.filterSweepRate() >>>;
    <<< "mod speed:", moog.lfoSpeed() >>>;
    <<< "mod depth:", moog.lfoDepth() >>>;
    <<< "volume:", moog.volume() >>>;
    <<< "vibrato freq:", moog.vibratoFreq() >>>;
    <<< "vibrato gain:", moog.vibratoGain() >>>;

    // set freq
    scale[Std.rand2(0,scale.cap()-1)] => int winner;
    57 + Std.rand2(0,2)*12 + winner => Std.mtof => moog.freq;
    // go
    .8 => moog.noteOn;

    // advance time
    .5::second => now;
}
