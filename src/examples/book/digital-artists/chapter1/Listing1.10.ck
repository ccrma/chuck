// Listing 1.10 Twinkle with floats

/* Sine Twinkle Music with float variables
by ChucK Programmer
January 2025 */

SinOsc s => dac;

220.0 => float twinkle;           // You can make a float variable for your twinkle pitch and use math
1.6818 * twinkle => float little; //    to compute the value of another variable to use later for little.

1 => int onGain;
0 => int offGain;

// Play one note
twinkle => s.freq;        // Turns twinkle note on (sets gain using 
onGain => s.gain;         //    onGain variable and advances time).
0.3 :: second => now;

offGain => s.gain;        // Turns off note (sets gain to offGain and advances time).
0.3 :: second => now;

1.5 *=> twinkle;          // Modifies twinkle pitch using math, so you
                          //    can do the 2nd, higher twinkle.

// Play other note of 2nd "twinkle"
twinkle => s.freq;
onGain => s.gain;
0.3 :: second => now;
offGain => s.gain;
0.3 :: second => now;

// Play one note of "little"
little => s.freq;
onGain => s.gain;
0.3 :: second => now;
offGain => s.gain;
0.3 :: second => now;
