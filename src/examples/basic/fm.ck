// FM synthesis by hand

// carrier
SinOsc c => dac;
// modulator
SinOsc m => blackhole;

// carrier frequency
220 => float cf;
// modulator frequency
550 => float mf => m.freq;
// index of modulation
200 => float index;

// time-loop
while( true )
{
    // modulate
    cf + (index * m.last()) => c.freq;
    // advance time by 1 samp
    1::samp => now;
}
