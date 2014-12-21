// Listing 1.1 Hello Sine!

SinOsc s => dac; // (1) Connects (ChucKs) a sine wav oscillator to the dac (sound out)
0.6 => s.gain;   // (2) Sets oscillator gain to 0.6
220 => s.freq;   // (3) Sets oscillator frequency to 220
second => now;   // (4) Lets it play for one second
