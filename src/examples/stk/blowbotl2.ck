// STK BlowBotl; using classic STK controlChange
// (also see blowbotl.ck)

// patch
BlowBotl bottle => dac;

// scale
[0, 2, 4, 7, 8, 11] @=> int scale[];

// infinite time loop
while( true )
{
    Math.random2f( 0, 128 ) => float noisegain;
    Math.random2f( 0, 128 ) => float vibratofreq;
    Math.random2f( 0, 128 ) => float vibratogain;
    Math.random2f( 0, 128 ) => float volume;

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
    scale[Math.random2(0,scale.size()-1)] + 57 => Std.mtof => bottle.freq;
    // go
    .8 => bottle.noteOn;

    // advance time
    1::second => now;
}
