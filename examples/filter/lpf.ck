// LPF is a low-pass filter
Noise n => LPF lpf => dac;

// infinite time-loop
while( true )
{
    // sweep the cutoff
    Math.sin(now/second) * 110 => Math.fabs => Std.mtof => lpf.freq;
    // advance time
    5::ms => now;
}
