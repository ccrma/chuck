// ensure an array can different subclasses of its declared type
[new SinOsc, new SinOsc, new TriOsc] @=> UGen a[];
[new SinOsc, new SinOsc, new SinOsc] @=> UGen b[];
[new SinOsc] @=> UGen c[];
UGen d[0];
[new SinOsc, new Event] @=> Object e[];

a << new Noise;
d << new Noise << new SinOsc;

if (a.size() != 4) <<< "test failed" >>>;
else if (d.size() != 2) <<< "test failed" >>>;
else <<< "success" >>>;
