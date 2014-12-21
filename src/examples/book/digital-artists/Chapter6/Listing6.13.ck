// Listing 6.13 Using ChucK's built-in NRev reverberator UGen

// make a new reverb and hook it up
// (Again, Beware Feedback!
// Turn down the volume or wear headphones)

adc => NRev rev => dac;

// set reverb/dry mixture
0.05 => rev.mix;

// kick back and enjoy the space
while (1) {
    1.0 :: second => now;
}
