// STK Saxofony

// patch
Saxofony sax => JCRev r => dac;
.5 => r.gain;
.05 => r.mix;

// our notes
[ 61, 63, 65, 66, 68 ] @=> int notes[];

// infinite time-loop
while( true )
{
    // set
    Std.rand2f( 0, 1 ) => sax.stiffness;
    Std.rand2f( 0, 1 ) => sax.aperture;
    Std.rand2f( 0, 1 ) => sax.noiseGain;
    Std.rand2f( 0, 1 ) => sax.blowPosition;
    Std.rand2f( 0, 12 ) => sax.vibratoFreq;
    Std.rand2f( 0, 1 ) => sax.vibratoGain;
    Std.rand2f( 0, 1 ) => sax.pressure;

    // print
    <<< "---", "" >>>;
    <<< "stiffness:", sax.stiffness() >>>;
    <<< "aperture:", sax.aperture() >>>;
    <<< "noiseGain:", sax.noiseGain() >>>;
    <<< "blowPosition:", sax.blowPosition() >>>;
    <<< "vibratoFreq:", sax.vibratoFreq() >>>;
    <<< "vibratoGain:", sax.vibratoGain() >>>;
    <<< "pressure:", sax.pressure() >>>;

    // factor
    Std.rand2f( .75, 2 ) => float factor;

    for( int i; i < notes.cap(); i++ )
    {
        play( 12 + notes[i], Std.rand2f( .6, .9 ) );
        300::ms * factor => now;
    }
}

// basic play function (add more arguments as needed)
fun void play( float note, float velocity )
{
    // start the note
    Std.mtof( note ) => sax.freq;
    velocity => sax.noteOn;
}
