// our patch
SqrOsc so => LPF f => dac;

// set osc frequency
50 => so.freq;
// set Q
10 => f.Q;
// set gain
.5 => f.gain;

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
