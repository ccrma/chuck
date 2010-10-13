// patch
Bowed bow => dac;

// scale
[0, 2, 4, 7, 8, 11] @=> int scale[];

// infinite time loop
while( true )
{
    // set
    Std.rand2f( 0, 1 ) => bow.bowPressure;
    Std.rand2f( 0, 1 ) => bow.bowPosition;
    Std.rand2f( 0, 12 ) => bow.vibratoFreq;
    Std.rand2f( 0, 1 ) => bow.vibratoGain;
    Std.rand2f( 0, 1 ) => bow.volume;

    // print
    <<< "---", "" >>>;
    <<< "bow pressure:", bow.bowPressure() >>>;
    <<< "bow position:", bow.bowPosition() >>>;
    <<< "vibrato freq:", bow.vibratoFreq() >>>;
    <<< "vibrato gain:", bow.vibratoGain() >>>;
    <<< "volume:", bow.volume() >>>;

    // set freq
    scale[Std.rand2(0,scale.cap()-1)] + 57 => Std.mtof => bow.freq;
    // go
    .8 => bow.noteOn;

    // advance time
    Std.rand2f(.8, 2)::second => now;
}
