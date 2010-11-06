// take me to your leader (talk into the mic)
// gewang, prc

// our patch - feedforward part
adc => Gain g => DelayL d => dac;
adc => Gain g2 => dac;
// feedback
d => Gain g3 => d;

// set parameters
15::ms => d.delay;
0.05 => g.gain;
0.05 => g2.gain;
0.95 => g3.gain;

// time loop
while( true ) 100::ms => now;
