// Listing 6.7 Simple plucked string physical model

// Super simple Karplus-Strong plucked string
Impulse imp => Delay str => dac;

// connect string back into itself
str => str;

// round-trip string delay, 100 Hz At 44.1k SRATE
441.0 :: samp => str.delay;

// set round-trip string gain to less than 1.0
0.98 => str.gain;

// "pluck" the string
1.0 => imp.next;      // (1) Tells the impulse to output a
                      //     1.0 (only for the next sample)

// let the string "ring" a bit
5.0 :: second => now;
