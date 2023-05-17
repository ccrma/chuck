Noise nse => WPKorg35 fltr => dac;

SinOsc sin => blackhole;
SinOsc sinb => blackhole;
0.01 => nse.gain;
40 => sin.freq;
0.0125 => sinb.freq;
1.99999 => fltr.resonance;
true => fltr.nonlinear;

while(true){
    40 * sinb.last() => sin.freq;
    40 + (Math.pow((sin.last() + 1)/2.0,2) * 17000) => fltr.cutoff;
    1::samp => now;
}
