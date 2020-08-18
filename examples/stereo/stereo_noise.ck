// the patch, note pan2 is stereo
Noise n => Pan2 p => dac;
.1 => n.gain;

// variable
0.0 => float t;
// smaller == smoother
10::ms => dur T;

// time loop
while( true )
{
    // pan goes from -1 (left) to 1 (right)
    Math.sin(t) => p.pan;
    // increment t (scaling by T)
    T / second * 2.5 +=> t;

    // uncomment to print out pan, last left, last right
    // <<<p.pan(), dac.left.last(), dac.right.last()>>>;

    // advancde time
    T => now;
}
