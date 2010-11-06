// STK BlowBotl; using classic STK controlChange
// (also see blowbotl.ck)

// patch
BlowBotl bottle => dac;

// scale
[0, 2, 4, 7, 8, 11] @=> int scale[];

// infinite time loop
while( true )
{
    Std.rand2f( 0, 128 ) => float noisegain;
    Std.rand2f( 0, 128 ) => float vibratofreq;
    Std.rand2f( 0, 128 ) => float vibratogain;
    Std.rand2f( 0, 128 ) => float volume;

    <<< "---", "" >>>;
    <<< "noise gain:", noisegain, "/ 128.0" >>>;
    <<< "vibrato freq:", vibratofreq, "/ 128.0" >>>;
    <<< "vibrato gain:", vibratogain, "/ 128.0" >>>;
    <<< "volume:", volume, "/ 128.0" >>>;

    // noise gain
    bottle.controlChange( 4, noisegain );
    // vibrato freq
    bottle.controlChange( 11, vibratofreq );
    // vibrato gain
    bottle.controlChange( 1, vibratogain );
    // volume
    bottle.controlChange( 128, volume );

    // set freq
    scale[Std.rand2(0,scale.cap()-1)] + 57 => Std.mtof => bottle.freq;
    // go
    .8 => bottle.noteOn;

    // advance time
    1::second => now;
}
