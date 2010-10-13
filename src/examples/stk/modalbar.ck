// STK ModalBar

// patch
ModalBar bar => dac;

// scale
[0, 2, 4, 7, 8, 11] @=> int scale[];

// infinite time loop
while( true )
{
    // ding!
    Std.rand2( 0, 8 ) => bar.preset;
    Std.rand2f( 0, 1 ) => bar.stickHardness;
    Std.rand2f( 0, 1 ) => bar.strikePosition;
    Std.rand2f( 0, 1 ) => bar.vibratoGain;
    Std.rand2f( 0, 60 ) => bar.vibratoFreq;
    Std.rand2f( 0, 1 ) => bar.volume;
    Std.rand2f( .5, 1 ) => bar.directGain;
    Std.rand2f( .5, 1 ) => bar.masterGain;

    // print
    <<< "---", "" >>>;
    <<< "preset:", bar.preset() >>>;
    <<< "stick hardness:", bar.stickHardness() >>>;
    <<< "strike position:", bar.strikePosition() >>>;
    <<< "vibrato gain:", bar.vibratoGain() >>>;
    <<< "vibrato freq:", bar.vibratoFreq() >>>;
    <<< "volume:", bar.volume() >>>;
    <<< "direct gain:", bar.directGain() >>>;
    <<< "master gain:", bar.masterGain() >>>;

    // set freq
	scale[Std.rand2(0,scale.cap()-1)] => int winner;
    57 + Std.rand2(0,2)*12 + winner => Std.mtof => bar.freq;
    // go
    .8 => bar.noteOn;

    // advance time
    .5::second => now;
}
