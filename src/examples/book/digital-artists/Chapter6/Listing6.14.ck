// Listing 6.14 Using ChucK's built-in pitch shifter UGen, PitShift

// run mic input through pitch shifter
adc => PitShift p => dac;

// set mix to all pitch shift (no dry signal)
1.0 => p.mix;

// forever shifting pitch
while (1)
{
    // pick a random shift across +/- 1 octave
    Math.random2f(0.5,2.0) => p.shift;
    0.2 :: second => now;
}

