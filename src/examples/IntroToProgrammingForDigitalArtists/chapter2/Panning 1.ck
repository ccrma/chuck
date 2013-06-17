// Set s to left speaker, and t to right
SinOsc s => dac.left;
SinOsc t => dac.right;

// Set Frequencies an octave apart
220.35 => s.freq;
330.67 => t.freq;

// Advance Time
1::second => now;


