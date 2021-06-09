// BPF is a band-pass filter
Noise n => BPF f => dac;

// set filter Q
1 => f.Q;

// infinite time-loop
while( true )
{
    // sweep the cutoff
    100 + Math.fabs(Math.sin(now/second)) * 5000 => f.freq;
    // advance time
    5::ms => now;
}
