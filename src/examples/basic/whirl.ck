// sine to dac
SinOsc s => dac;
// let's turn down gain, for this can be loud and annoying
.15 => s.gain;

// infinite time loop
0.0 => float t;
while( true )
{
    // modulate
    30 + ( Math.sin(t) + 1.0 ) * 10000.0 => s.sfreq;
    t + .004 => t;

    // advance time
    1::ms => now;
}
