// plucked string filter, different excitation
// Ge Wang (gewang@cs.princeton.edu)

// feedforward
SndBuf buffy => PoleZero block => OneZero lowpass => dac;
// feedback
lowpass => Delay delay => lowpass;

// our radius
.99999 => float R;
// our delay order
250 => float L;
// set delay
L::samp => delay.delay;
// set dissipation factor
Math.pow( R, L ) => delay.gain;
// take out DC and neighborhood
.999 => block.blockZero;
// place zero
-1 => lowpass.zero;

// fire excitation (try other sounds too)
"special:mand1" => buffy.read;

// advance time
(Math.log(.0001) / Math.log(R))::samp => now;
