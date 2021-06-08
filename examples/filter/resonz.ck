// ResonZ is a second-order resonance filter with equal-gain zeroes.
// This filter keeps its gain in-check across frequencies, including
// near the extremes; useful for sweeping
Noise n => ResonZ f => dac;

// set filter Q (higher == narrower, sharper resonance)
2 => f.Q;

// infinite time-loop
while( true )
{
    // sweep the cutoff
    100 + Math.fabs(Math.sin(now/second)) * 5000 => f.freq;
    // advance time
    5::ms => now;
}
