// Listing 5.15 Recursive (function that calls itself) scale-playing function

// sound chain, mandolin to audio out
Mandolin mand => dac;                    // (1) Mandolin instrument

// recursive scale player
fun int recurScale(int note, dur rate) { // (2) recurScale function definition
    Std.mtof(note) => mand.freq;         // (3) Sets frequency of mandolin note
    1 => mand.noteOn;                    // (4) Plays note using noteOn
    rate => now;                         // (5) Waits for duration rate 

    // only do it until some limit is reached
    if (note > 40)                       // (6) Limit for recursion
    {
        // here's the recursion, it calls itself!
        recurScale(note-1, 0.9*rate);    // (7) recurScale can call recurScale!
    }
}

// now play a couple of scales
recurScale(60, 0.5 :: second);
recurScale(67, 1.0 :: second);
