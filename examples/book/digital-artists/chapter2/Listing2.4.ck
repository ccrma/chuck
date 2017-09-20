// Listing 2.4 Using dac.left and dac.right to connect to left and right speakers

// two sine waves in stereo
SinOsc s => dac.left;  // (1) Connects one SinOsc to the left channel...
SinOsc t => dac.right; // (2) ...and another to the right channel.

// set frequencies
220 => s.freq;        // (3) Sets the frequency of the left osc...
361 => t.freq;        // (4) and sets the right osc to a different frequency.

// advance time
second => now;
