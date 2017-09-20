// FM synthesis (using Step instead of .sync == 2; see fm2.ck)

// modulator to carrier
SinOsc m => SinOsc c => dac;
// step function, add to modulator output
Step step => c;

// carrier frequency
440 => step.next;
// modulator frequency
110 => m.freq;
// index of modulation
300 => m.gain;

// time-loop
while( true ) 1::second => now;
