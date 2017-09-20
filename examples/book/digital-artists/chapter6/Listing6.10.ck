// Listing 6.10 Simple resonant-filtered impulse makes for cool computer music

// Computer music!! (1) Impulse through resonant filter
Impulse imp => ResonZ filt => dac;

// Set the Q ((2) Quality, amount of resonance) 
100.0 => filt.Q; // fairly high, to yield a pitch

while (1)
{
    // pick a random frequency
    Math.random2f(500.0,2500.0) => filt.freq;

    // fire our impulse, and hang out a bit
    100.0 => imp.next;    // (3) Tells impulse to output 100.0
                          //    (only on next sample).
    0.1 :: second => now;
}

