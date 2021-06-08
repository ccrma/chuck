// our patch
SqrOsc so => LPF f => dac;

// set osc frequency
50 => so.freq;
// set Q
10 => f.Q;
// set gain
.5 => f.gain;

// infinite time-loop
while( true )
{
    // sweep the cutoff
    100 + Math.fabs(Math.sin(now/second)) * 5000 => f.freq;
    // advance time
    5::ms => now;
}
