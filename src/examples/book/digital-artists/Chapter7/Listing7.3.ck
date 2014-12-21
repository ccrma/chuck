// Listing 7.3 Noodling around with the STK Flute PM UGen

// Noodling around with the STK Flute UGen
Flute pipe => dac;

while (true)
{
    // pick a random MIDI note to play
    Std.mtof(Math.random2(60,80)) => pipe.freq;
    1 => pipe.noteOn;
    0.8 :: second => now;

    // then randomly mess around with the jetDelay
    for (0 => int i; i < 4; i++) {
        Math.random2f(0.0,1.0) => pipe.jetDelay;
        0.2 :: second => now;
    }

    1 => pipe.noteOff;
    0.1 :: second => now;
}

