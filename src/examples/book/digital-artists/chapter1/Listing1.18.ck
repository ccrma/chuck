// Listing 1.18 Musical while loop

SinOsc s => dac;

// initialize variable i
20 => int i;            // (1) Initializes the counter/frequency to 20.

// while loop (instead of for loop)
while ( i < 400)        // (2) A while loop only has a conditional test
{                       // (3) Block of code to execute, including...
    <<< i >>>;
    i => s.freq;
    10::ms => now;
    i++;                // (4) ...increment counter (very important!!)
}
