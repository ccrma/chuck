// FeatureCollector is really meant to be used for recording data for machine
// learning analysis (see https://chuck.cs.princeton.edu/doc/language/uana.html),
// but this example shows barebons usage of the UAana for something silly.
//
// author: Curtis Ullerich

FeatureCollector fc =^ pilF pilf => dac;
SinOsc one => Flip f1 =^ fc;
TriOsc two => Flip f2 =^ fc;
.5 => one.gain => two.gain;

300 => int freq;
freq => one.freq;
2*freq => two.freq;
second/freq => dur period;
(period/samp)$int => int size; // number of samples in a period
size => f1.size => f2.size;

while(true) {
  period => now;
  pilf.upchuck();
}
