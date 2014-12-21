// Listing 1.14 if/else code example

// sound chain
SinOsc s => dac;            // Sine to dac, set frequency and gain.

// set frequency
220.0 => s.freq;
// set volume
0.5 => s.gain;

// chance logical variable // (1) New integer named chance,
3 => int chance;           //     initialized to value of 3.

if (chance == 1)           // (2) If chance is equal to 1...
{
    // play first "twinkle" note if chance == 1
    1 :: second => now;    // (3) ...do this block...
}
else     // (4) ...otherwise (else), set sine frequency to something 
{        //        different and play for a longer time.
    // otherwise, play other, higher twinkle note
    330.0 => s.freq;
    // and play it for a much longer time
    3::second => now;
}

// set and play one note, an octave above "twinkle"
440.0 => s.freq;        // After the if/else, play another, much higher note
1 :: second => now;
