// STK BlowBotl

// patch
BlowBotl bottle => dac;

// scale
[0, 2, 4, 7, 8, 11] @=> int scale[];

// infinite time loop
while( true )
{
    // ding!
    Std.rand2f( 0, 1 ) => bottle.noiseGain;
    Std.rand2f( 0, 12 ) => bottle.vibratoFreq;
    Std.rand2f( 0, 1 ) => bottle.vibratoGain;
    Std.rand2f( 0, 1 ) => bottle.volume;

    // print
    <<< "---", "" >>>;
    <<< "noise gain:", bottle.noiseGain() >>>;
    <<< "vibrato freq:", bottle.vibratoFreq() >>>;
    <<< "vibrato gain:", bottle.vibratoGain() >>>;
    <<< "volume:", bottle.volume() >>>;

    // set freq
    scale[Std.rand2(0,scale.cap()-1)] + 57 => Std.mtof => bottle.freq;
    // go
    .8 => bottle.noteOn;

    // advance time
    1::second => now;
}
