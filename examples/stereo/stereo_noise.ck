// white noise to Pan2 (stereo) to dac
Noise n => Pan2 p => dac;
// pan type; 1 is equal power panning
// <<< "pan type:", p.panType() >>>;
// not too loud
.1 => n.gain;

// variable
0.0 => float t;
// smaller == smoother
10::ms => dur T;

// print
<<< "holding pan at center for 1 second...", "" >>>;
// hold the pan to be in the middle first
1::second => now;

// print
<<< "panning left <-> right...", "" >>>;
// time loop
while( true )
{
    // pan goes from -1 (left) to 1 (right)
    Math.sin(t) => p.pan;
    // increment t (scaling by T)
    T / second * 2.5 +=> t;
    // advancde time
    T => now;
}
