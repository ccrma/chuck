// Listing 6.11 Testing the LPF resonant low pass filter with noise input

// pass noise through low pass filter
Noise nz => LPF lp => dac;

// set frequency and Q
500.0 => lp.freq;
100.0 => lp.Q;
0.2 => lp.gain;
second => now;
