// our patch
Noise n => BPF f => dac;

// set filter Q
1 => f.Q;

// infinite time-loop
float t;
while( true )
{
    // sweep the cutoff
    100 + Std.fabs(Math.sin(t)) * 5000 => f.freq;
    // increment t
    .005 +=> t;
    // advance time
    5::ms => now;
}
