// Listing 5.7 Using a swell function to ramp oscillator volume up and down

// swell function, operates on any type of UGen
fun void swell(UGen osc, float begin, float end, float step)
{                                  // (1) swell function definition
    float val;

    // swell up volume
    for (begin => val; val < end; step +=> val)  // (2) for loop to ramp up volume
    {
        val => osc.gain;
        0.01 :: second => now;
    }

    // swell down volume
    while (val > begin)                          // (3) while loop to ramp it back down
    {
        val => osc.gain;
        step -=> val;
        0.01:: second => now;
    }
}

<<< "This code does nothing by itself.", "Run Listing5.8.ck" >>>;

