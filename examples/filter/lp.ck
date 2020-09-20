// our patch
Noise n => LPF lpf => dac;

// infinite time-loop
float t;
while( true )
{
    // sweep the cutoff
    Math.sin(t) * 110 => Std.fabs => Std.mtof => lpf.freq;
    // increment t
    .005 +=> t;
    // advance time
    5::ms => now;
}
