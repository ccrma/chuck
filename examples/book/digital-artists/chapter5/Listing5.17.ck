// Listing 5.17 Sonifying the factorial() function

SinOsc s => dac;                    //(1) SinOsc so you can hear factorial
// our recursive factorial function
fun int factorial(int x)            // (2) factorial function definition
{
    sonify(x);                      // (3) Call the sonify function within factorial
    if (x <=1) return 1;
    else return (x*factorial(x-1));
}

// a function to sonify numbers
fun void sonify(int note) {         // (4) Definition of the sonify function
    // offset above middle C
    Std.mtof(60+(0.5*note)) => s.freq; // (5) Sets frequency as function of note
    1.0 => s.gain;                  // (6) Turns on Osc
    300 :: ms => now;
    0.0 => s.gain;
    50 :: ms => now;
}

// try it out!
sonify(factorial(2));
second => now;

sonify(factorial(3));
second => now;

sonify(factorial(4));
second => now;

sonify(factorial(5));
second => now;

