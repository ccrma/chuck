// Listing 1.19 Using more than one oscillator

// connect two oscillators to the dac
SinOsc s => dac;            // Our usual sine wave
SinOsc s2 => dac;           // (1) Another sine wave oscillator

// set their frequencies and gains
220 => s.freq;              // (2) Sets frequency of first sine
1030 => s2.freq;            // (3) Sets frequency of second sine
0.5 => s.gain;              // (4) Sets gains to 1/2
0.5 => s2.gain;

second => now;              // (5) Lets time pass so you can hear the sound.


