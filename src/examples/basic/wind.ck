// noise generator, biquad filter, dac (audio output) 
Noise n => BiQuad f => dac;
// set biquad pole radius
.99 => f.prad;
// set biquad gain
.05 => f.gain;
// set equal zeros 
1 => f.eqzs;
// our float
0.0 => float t;

// infinite time-loop
while( true )
{
    // sweep the filter resonant frequency
    100.0 + Std.fabs(Math.sin(t)) * 15000.0 => f.pfreq;
    t + .01 => t;
    // advance time
    5::ms => now;
}
