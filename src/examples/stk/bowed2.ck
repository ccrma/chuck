// patch
Bowed bow => dac;

// scale
[0, 2, 4, 7, 8, 11] @=> int scale[];

// infinite time loop
while( true )
{
    Math.random2f( 0, 128 ) => float pressure;
    Math.random2f( 0, 128 ) => float position;
    Math.random2f( 0, 128 ) => float vibratofreq;
    Math.random2f( 0, 128 ) => float vibratogain;
    Math.random2f( 0, 128 ) => float volume;

    <<< "---", "" >>>;
    <<< "bow pressure:", pressure >>>;
    <<< "bow position:", position >>>;
    <<< "vibrato freq:", vibratofreq >>>;
    <<< "vibrato gain:", vibratogain >>>;
    <<< "volume:", volume >>>;

    // bow pressure
    bow.controlChange( 2, pressure );
    // bow position
    bow.controlChange( 4, position );
    // vibrato freq
    bow.controlChange( 11, vibratofreq );
    // vibrato gain
    bow.controlChange( 1, vibratogain );
    // volume
    bow.controlChange( 128, volume );

    // set freq
    scale[Math.random2(0,scale.size()-1)] + 57 => Std.mtof => bow.freq;
    // go
    .8 => bow.noteOn;

    // advance time
    Math.random2f(.8, 2)::second => now;
}
