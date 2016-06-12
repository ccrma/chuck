// patch
Bowed bow => dac;

// scale
[0, 2, 4, 7, 8, 11] @=> int scale[];

// infinite time loop
while( true )
{
    // set
    Math.random2f( 0, 1 ) => bow.bowPressure;
    Math.random2f( 0, 1 ) => bow.bowPosition;
    Math.random2f( 0, 12 ) => bow.vibratoFreq;
    Math.random2f( 0, 1 ) => bow.vibratoGain;
    Math.random2f( 0, 1 ) => bow.volume;

    // print
    <<< "---", "" >>>;
    <<< "bow pressure:", bow.bowPressure() >>>;
    <<< "bow position:", bow.bowPosition() >>>;
    <<< "vibrato freq:", bow.vibratoFreq() >>>;
    <<< "vibrato gain:", bow.vibratoGain() >>>;
    <<< "volume:", bow.volume() >>>;

    // set freq
    scale[Math.random2(0,scale.size()-1)] + 57 => Std.mtof => bow.freq;
    // go
    .8 => bow.noteOn;

    // advance time
    Math.random2f(.8, 2)::second => now;
}
