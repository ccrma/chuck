// ensure an array can different subclasses of its declared type
[new SinOsc, new SinOsc, new TriOsc] @=> UGen a[];
[new SinOsc, new SinOsc, new TriOsc] @=> Osc b[];
[new SinOsc, new SinOsc, new SinOsc] @=> UGen c[];
[new SinOsc] @=> UGen d[];
UGen e[0];
[new SinOsc, new Event] @=> Object f[];

a << new Noise;
e << new Noise << new SinOsc;

if (a.size() != 4) <<< "test failed" >>>;
else if (e.size() != 2) <<< "test failed" >>>;
else <<< "success" >>>;
