Noise nse => WPDiodeLadder fltr => dac;

SinOsc sin => blackhole;
SinOsc sinb => blackhole;
0.01 => nse.gain;
40 => sin.freq;
0.0125 => sinb.freq;
17 => fltr.resonance;
true => fltr.nonlinear;
false => fltr.nlp_type;

while(true){
    40 * sinb.last() => sin.freq;
    40 + (Math.pow((sin.last() + 1)/2.0,2) * 17000) => fltr.cutoff;
    1::samp => now;
}
