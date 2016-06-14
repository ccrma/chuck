// bandlimited ugens (Blit, BlitSaw, BlitSquare)
Blit s => Pan2 p;
p.left => JCRev r1 => dac.left;
p.right => JCRev r2 => dac.right;

// initial settings
.5 => s.gain;
.1 => r1.mix;
.1 => r2.mix;

// an array
[ 0, 2, 4, 7, 9, 11 ] @=> int hi[];

// set the harmonic
4 => s.harmonics;

// spork the pan control
spork ~ dopan();

// infinite time loop
while( true )
{
    Std.mtof( 33 + Math.random2(0,3) * 12 +
        hi[Math.random2(0,hi.size()-1)] ) => s.freq;
    120::ms => now;
}

// pan control
fun void dopan()
{
    float t;
    while( true )
    {
        .7 * Math.sin(t) => p.pan;
        .005 +=> t;
        10::ms => now;
    }
}
