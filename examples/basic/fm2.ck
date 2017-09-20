// basic FM synthesis using SinOsc (2 => .sync; also see fm3.ck)

// modulator to carrier
SinOsc m => SinOsc c => dac;

// carrier frequency
440 => c.freq;
// modulator frequency
110 => m.freq;
// index of modulation
300 => m.gain;

// phase modulation is FM synthesis (sync is 2)
2 => c.sync;

// time-loop
while( true ) 1::second => now;
