// karplus + strong plucked string filter
// Ge Wang (gewang@cs.princeton.edu)

// feedforward
Noise imp => OneZero lowpass => dac;
// feedback
lowpass => Delay delay => lowpass;

// our radius
.99999 => float R;
// our delay order
500 => float L;
// set delay
L::samp => delay.delay;
// set dissipation factor
Math.pow( R, L ) => delay.gain;
// place zero
-1 => lowpass.zero;

// fire excitation
1 => imp.gain;
// for one delay round trip
L::samp => now;
// cease fire
0 => imp.gain;

// advance time
(Math.log(.0001) / Math.log(R))::samp => now;
