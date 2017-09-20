// our patch
SqrOsc so => BRF f => dac;

// set osc frequency
50 => so.freq;
// set filter Q
.25 => f.Q;
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
