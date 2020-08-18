// Listing 6.9 Even better plucked string, with enveloped noise and low-pass filter

// Even Better Karplus-Strong plucked string
// Noise through ADSR into interpolating delay line

Noise nois => ADSR pluck => DelayA str => dac;

// hook string back into itself
// Feedback delay through a low-pass loop filter
str => OneZero lowPass => str;

// set ADSR noise envelope parameters
// Sets ADSR parameters to pluck rapidly and then stick at 0.0
pluck.set(0.002 :: second, 0.002 :: second, 0.0, 0.01 :: second);

// Play random notes forever
while (true)
{
           // Can now set delay length to any arbitrary float number
    Math.random2f(110.0, 440.0) :: samp => str.delay;

    // turn on note (pluck string) and wait a bit
    1 => pluck.keyOn;  // Plucks by sending keyOn to float number ADSR, 
                       // gates noise into string
    0.3 :: second => now;
}


