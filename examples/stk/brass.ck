// patch
Brass brass => JCRev r => dac;
.75 => r.gain;
.05 => r.mix;

// our notes
[ 61, 63, 65, 66, 68 ] @=> int notes[];

// infinite time-loop
while( true )
{
    // set
    Math.random2f( 0, 1 ) => brass.lip;
    Math.random2f( 0, 1 ) => brass.slide;
    Math.random2f( 0, 12 ) => brass.vibratoFreq;
    Math.random2f( 0, 1 ) => brass.vibratoGain;
    Math.random2f( 0, 1 ) => brass.volume;

    // print
    <<< "---", "" >>>;
    <<< "lip tension:", brass.lip() >>>;
    <<< "slide length:", brass.slide() >>>;
    <<< "vibrato freq:", brass.vibratoFreq() >>>;
    <<< "vibrato gain:", brass.vibratoGain() >>>;
    <<< "volume:", brass.volume() >>>;

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
    Std.mtof( note ) => brass.freq;
    velocity => brass.noteOn;
}
