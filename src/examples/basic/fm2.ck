// basic FM synthesis using sinosc

// modulator to carrier
SinOsc m => SinOsc c => dac;

// carrier frequency
220 => c.freq;
// modulator frequency
20 => m.freq;
// index of modulation
200 => m.gain;

// phase modulation is FM synthesis (sync is 2)
2 => c.sync;

// time-loop
while( true ) 1::second => now;
