// our patch
Noise n => HPF f => dac;

// set gain
.5 => f.gain;

// infinite time-loop
float t;
while( true )
{
    // sweep the cutoff
    Math.sin(t) * 110 => Std.fabs => Std.mtof => f.freq;
    // increment t
    .005 +=> t;
    // advance time
    5::ms => now;
}
