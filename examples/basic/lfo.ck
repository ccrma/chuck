// low-frequency oscillator

// sine wave to blackhole (like dac but no sound)
SinOsc lfo => blackhole;
// set period (an alternative to .freq)
1::second => lfo.period;

// infinite time loop
while( true )
{
    // print out last value
    <<< lfo.last(), "" >>>;
    // advance time
    50::ms => now;
}
