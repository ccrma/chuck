// another candidate for least exciting demo

// patch
SinOsc s => JCRev r => dac;
.5 => r.gain;
.075 => r.mix;

// note number
20 => float note;

// go up to 127
while( note < 128 )
{
    // convert MIDI note to hz
    Std.mtof( note ) => s.freq;
    // turn down the volume gradually
    .5 - (note/256.0) => s.gain;

    // move up by whole step
    note + 2 => note;

    // advance time
    .125::second => now;
}

// turn off s
0 => s.gain;
// wait a bit
2::second => now;

