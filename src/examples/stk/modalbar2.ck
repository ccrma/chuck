// STK ModalBar

// patch
ModalBar bar => dac;

// scale
[0, 2, 4, 7, 8, 11] @=> int scale[];

// infinite time loop
while( true )
{
    // ding!
    Std.rand2( 0, 8 ) => int preset;
    Std.rand2f( 0, 128 ) => float stickHardness;
    Std.rand2f( 0, 128 ) => float strikePosition;
    Std.rand2f( 0, 128 ) => float vibratoGain;
    Std.rand2f( 0, 128 ) => float vibratoFreq;
    Std.rand2f( 0, 128 ) => float volume;
    Std.rand2f( 64, 128 ) => float directGain;
    Std.rand2f( 64, 128 ) => float masterGain;

    bar.controlChange( 2, stickHardness );
    bar.controlChange( 4, strikePosition );
    bar.controlChange( 11, vibratoGain );
    bar.controlChange( 7, vibratoFreq );
    bar.controlChange( 1, directGain);
    bar.controlChange( 128, volume );
    bar.controlChange( 16, preset );

    <<< "---", "" >>>;
    <<< "preset:", preset >>>;
    <<< "stick hardness:", stickHardness, "/ 128.0" >>>;
    <<< "strike position:", strikePosition, "/ 128.0" >>>;
    <<< "vibrato gain:", vibratoGain, "/ 128.0" >>>;
    <<< "vibrato freq:", vibratoFreq, "/ 128.0" >>>;
    <<< "volume:", volume, "/ 128.0" >>>;
    <<< "direct gain:", directGain, "/ 128.0" >>>;
    <<< "master gain:", masterGain, "/ 128.0" >>>;

    // set freq
	scale[Std.rand2(0,scale.cap()-1)] => int winner;
    57 + Std.rand2(0,2)*12 + winner => Std.mtof => bar.freq;
    // go
    .8 => bar.noteOn;

    // advance time
    .5::second => now;
}
