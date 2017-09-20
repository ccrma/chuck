// STK Clarinet
// (also see examples/event/polyfony2.ck)

// patch
Clarinet clair => JCRev r => dac;
.75 => r.gain;
.1 => r.mix;

// our notes
[ 61, 63, 65, 66, 68, 66, 65, 63, 61 ] @=> int notes[];

// infinite time-loop
while( true )
{
    Math.random2f( 64, 128 ) => float stiffness;
    Math.random2f( 0, 128 ) => float noisegain;
    Math.random2f( 0, 128 ) => float vibratofreq;
    Math.random2f( 0, 128 ) => float vibratogain;
    Math.random2f( 64, 128 ) => float pressure;

    <<< "---", "" >>>;
    <<< "reed stiffness:", stiffness >>>;
    <<< "noise gain:", noisegain >>>;
    <<< "vibrato freq:", vibratofreq >>>;
    <<< "vibrato gain:", vibratogain >>>;
    <<< "breath pressure:", pressure >>>;

    // clear
    clair.clear( 1.0 );

    // reed stiffness
    clair.controlChange( 2, stiffness );
    // noise gain
    clair.controlChange( 4, noisegain );
    // vibrato freq
    clair.controlChange( 11, vibratofreq );
    // vibrato gain
    clair.controlChange( 1, vibratogain );
    // breath pressure
    clair.controlChange( 128, pressure );

    for( int i; i < notes.size(); i++ )
    {
        play( 12 + notes[i], Math.random2f( .6, .9 ) );
        300::ms => now;
    }
}

// basic play function (add more arguments as needed)
fun void play( float note, float velocity )
{
    // start the note
    Std.mtof( note ) => clair.freq;
    velocity => clair.noteOn;
}
