// name: honkeytonk-algo1.ck
// desc: ALL NEW! HonkyTonk 4op FM (TX81Z Algorithm 1)
//       This Algorithm can also do PumpOrgn, NylonGuit, others
//       See nylon-guitar-algo1.ck for how to do this
//
// author: Perry R. Cook
// date: June 2021, for REPAIRATHON 2021
//       needs chuck 1.4.1.0 or above

// to learn more about HnkyTonk, uncomment this:
// HnkyTonk.apropos();

// ugens!
HnkyTonk h[8];
Pan2 p[8];
Chorus c[2] => NRev r[2] => dac;
// turn volume down a bit
0.33 => r[0].gain => r[1].gain;
0.07 => c[0].mix;
0.07 => c[1].mix;
0.04 => r[0].mix;
0.04 => r[1].mix;

for (int i; i < 8; i++) {
    h[i] => p[i] => c;
    -0.4 + 0.2*i => p[i].pan;
    Math.random2f(6.95,7.05) => h[i].lfoSpeed;
    0.04 => h[i].lfoDepth;
}

Std.mtof(36) => h[0].freq;
Std.mtof(41) => h[1].freq;
Std.mtof(53) => h[2].freq;
Std.mtof(57) => h[3].freq;
Std.mtof(60) => h[4].freq;

[69,72,74,76,77,81] @=> int melody[];

// triplets (bass 1, bass 2, repeats)
// must contain 3-multiple of elements
[41, 48, 4,
 41, 48, 4,
 38, 50, 2,
 34, 53, 2]@=> int bass[];

// bass counter
0 => int n;
// drop amount
0 => int drop;
// start with bass line
bassLine( bass[0], bass[1], 4 );

// play melody concurrently over...
spork ~ doMelody();
// ... the bass line
while( true )
{
    // play current chord
    bassLine( bass[n*3] + drop, bass[n*3+1], bass[n*3+2] );
    // increment counter
    n++;
    // reset drop
    0 => drop;
    // end of cycle? (if yes, start over but with a drop)
    if( n >= bass.size()/3 ) { 0 => n; -12 => drop; }
}

fun void doMelody()
{
    while( true )
    {
        Std.mtof(melody[Math.random2(0,5)]) => h[5].freq;
        Math.random2f(.75,1) => h[5].noteOn;
        if (maybe*maybe) {
            Std.mtof(melody[Math.random2(0,5)]) => h[6].freq;
            Math.random2f(.5,.75) => h[6].noteOn;
        }
        Math.random2(1,3)*second/8 => now;
    }
}

fun void bassLine( int note1, int note2, int thisManyTimes )
{
    // set thumb1 note
    Std.mtof(note1) => h[1].freq;
    // repeat
    repeat( thisManyTimes )
    {
        1 => h[1].noteOn;
        second/2 => now;
        .75 => h[2].noteOn;
        .75 => h[3].noteOn;
        .75 => h[4].noteOn;
        second/2 => now;
        if (maybe*maybe) 
            Std.mtof(note2) => h[0].freq;
        else
            Std.mtof(note2-12) => h[0].freq;
        1 => h[0].noteOn;
        second/2 => now;
        .75 => h[2].noteOn;
        .75 => h[3].noteOn;
        .75 => h[4].noteOn;
        second/2 => now;
    }
}
