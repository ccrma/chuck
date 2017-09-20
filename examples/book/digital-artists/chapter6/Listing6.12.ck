// Listing 6.12 Simple reverb using three Delay UGens

// Direct sound
adc => Gain input => dac; // (1) Direct signal from adc to dac (through Gain)
1.0 => input.gain;

// Delay lines to model walls + ceiling
                          // (2) adc to dac via three delay lines in parallel
input => Delay d1 => dac;
input => Delay d2 => dac;
input => Delay d3 => dac;

// Hook delay lines back to themselves
//      (3) Closes each delay loop (hook output to input)
d1 => d1;
d2 => d2;
d3 => d3;

// set feedback/loss on all delay lines
0.6 => d1.gain => d2.gain => d3.gain; // (4) Can set all three delay
                                      //     gains in one line of code

// allocate memory and set delay lengths
0.06 :: second => d1.max => d1.delay; // (5) Can set max and delay
0.08 :: second => d2.max => d2.delay; //     all in one line of code each
0.10 :: second => d3.max => d3.delay;

// Enjoy the room you built!
while (1) {
    1.0 :: second => now;
}
