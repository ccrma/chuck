// Listing 6.8 Better plucked string physical model, excited with noise

// Better Karplus-Strong plucked string
Noise pluck => Delay str => dac;

// hook string back into itself
str => str;

// round-trip string delay, 100 Hz At 44.1k SRATE
441.0 :: samp => str.delay;

// set round-trip string gain to less than 1.0
0.98 => str.gain;

// "pluck" the string for the right amount of time
1.0 => pluck.gain;
441.0 :: samp => now;

// shut off the noise generator
0.0 => pluck.gain;

// let the string "ring" a bit
5.0 :: second => now;
