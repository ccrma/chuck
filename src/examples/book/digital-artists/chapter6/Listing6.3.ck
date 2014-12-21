// Listing 6.3 Sci-fi techno dance bass line using the PulseOsc UGen

//PulsOsc for techno-bass, by ChucK Programmer, 2014
PulseOsc p => dac; // connect a new PulseOsc to dac

// infinite loop of sci-fi techno!
while (true)
{
    Math.random2f(0.01,0.5) => p.width; // set random pulse width
    if (Math.random2(0,1))              // pick a pitch randomly
    {
        84.0 => p.freq;                 // from one of
    }
    else
    {
        100.0 => p.freq;                // two different pitches
    }

    1 => p.gain;                        // turn on oscillator
    0.06 :: second => now;              // hang out a bit

    0.0 => p.gain;                      // turn off oscillator
    0.04 :: second => now;              // hang out a bit
}
