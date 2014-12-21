//  WowExample.ck
//  (ChucK is a real programming language)
//  Type this in for instant gratification
//  Hit "Clear VM" to stop it

Impulse imp => ResonZ filt => NRev rev => dac;
0.04 => rev.mix;
100.0 => filt.Q => filt.gain;

while (1) {
    Std.mtof(Math.random2(60,84)) => filt.freq;
    1.0 => imp.next;
    100::ms => now;
}

