// sound chain
SinOsc s => dac;             // The usual sound patch.

// set frequency             // Sets frequency and gain.
220.0 => s.freq;
// set volume
0.6 => s.gain;

// chance logical variable
1 => int chance;    // (1) Defines an integer named chance to use as a logical 
                    //     variable (will have a value of either 1 or 0).

if (chance == 1)    // (2) if statement.
{
    // sound will play only if chance == 1
    1 :: second => now; // (3) Advances time and lets sound happen if true.
}

// set new frequency
330.0 => s.freq;        // Sets new frequency for a different note...
1 :: second => now;     // (4) ...then plays this new note.
