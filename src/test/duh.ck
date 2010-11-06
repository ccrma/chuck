sinosc s => dac;
.25 => s.gain;

// note number
10 => float note;

// go up to 127
while( note < 128 )
{
    // convert MIDI note to hz
    std.mtof( note ) => s.freq;

    // move up by whole step
    note + 2 => note;

    // advance time
    .125::second => now;
}
