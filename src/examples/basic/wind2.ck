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

// concurrent control
fun void wind_gain( )
{
    0.0 => float g; 

    // time loop to ramp up the gain / oscillate
    while( true )
    {
        Std.fabs( Math.sin( g ) ) => n.gain;
        g + .001 => g;
        10::ms => now;
    }
}

// run wind_gain on anothre shred
spork ~ wind_gain();

// infinite time-loop
while( true )
{
    // sweep the filter resonant frequency
    100.0 + Std.fabs(Math.sin(t)) * 15000.0 => f.pfreq;
    t + .01 => t;
    // advance time
    100::ms => now;
}
