// Listing 1.17 Musical use of a for loop

// set up our patch
SinOsc s => dac;               // Sine wave to dac

// a musical for loop         
for (20 => int i; i < 400; i++) // (1) For loop from 20 to 399
{
    <<< i >>>;                  // (2) Prints out current value
    i => s.freq;                // (3) Sets frequency to i
    10::ms => now; // advance time // (4) Waits 10ms, then loops again
}

