// HPF is a high-pass filter
Noise n => HPF f => dac;

// set gain
.5 => f.gain;

// infinite time-loop
while( true )
{
    // sweep the cutoff
    Math.sin(now/second) * 110 => Math.fabs => Std.mtof => f.freq;
    // advance time
    5::ms => now;
}
