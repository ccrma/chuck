// a simple comb filter
// Ge Wang (gewang@cs.princeton.edu)

// feedforward
Impulse imp => Gain out => dac;
// feedback
out => Delay delay => out;

// our radius
.99999 => float R;
// our delay order
500 => float L;
// set delay
L::samp => delay.delay;
// set dissipation factor
Math.pow( R, L ) => delay.gain;

// fire impulse
1 => imp.next;

// advance time
(Math.log(.0001) / Math.log(R))::samp => now;
