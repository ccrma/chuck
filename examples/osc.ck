// connect sine oscillator to audio output
sinosc s => dac;

// infinite time-loop
while(true)
{
    // randomly set a frequency
    std.rand2f( 30.0, 1000.0 ) => s.freq;
    // advance time
    100::ms => now;
}
