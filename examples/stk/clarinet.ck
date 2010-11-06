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
    // clear
    clair.clear( 1.0 );

    // set
    Std.rand2f( 0, 1 ) => clair.reed;
    Std.rand2f( 0, 1 ) => clair.noiseGain;
    Std.rand2f( 0, 12 ) => clair.vibratoFreq;
    Std.rand2f( 0, 1 ) => clair.vibratoGain;
    Std.rand2f( 0, 1 ) => clair.pressure;

    // print
    <<< "---", "" >>>;
    <<< "reed stiffness:", clair.reed() >>>;
    <<< "noise gain:", clair.noiseGain() >>>;
    <<< "vibrato freq:", clair.vibratoFreq() >>>;
    <<< "vibrato gain:", clair.vibratoGain() >>>;
    <<< "breath pressure:", clair.pressure() >>>;

    for( int i; i < notes.cap(); i++ )
    {
        play( 12 + notes[i], Std.rand2f( .6, .9 ) );
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
