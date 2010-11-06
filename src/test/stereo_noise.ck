// the patch, note pan2 is stereo
noise n => pan2 p => dac;

// variable
0.0 => float t;

// time loop
while( true )
{
    // pan goes from -1 (left) to 1 (right)
    math.sin(t) => p.pan;
    // increment t
    .25 +=> t;
    // print out pan, last left, last right
    <<<p.pan(), dac.left.last(), dac.right.last()>>>;
    // advancde time
    100::ms => now;
}
