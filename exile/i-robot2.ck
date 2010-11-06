// take me to your leader (talk into the mic)
// gewang, prc

// our patch - feedforward part
impulse i => gain g => DelayL d => dac;
i => gain g2 => dac;
adc => gain g4 => dac;
0.0 => g4.gain;
// feedback
d => gain g3 => d;
// set parameters
15::ms => d.delay;
0.05 => g.gain;
0.025 => g2.gain;
0.95 => g3.gain;

// time loop
while( true )
{
    3.0 * adc.last * adc.last => i.next;
    1::samp => now;
}
