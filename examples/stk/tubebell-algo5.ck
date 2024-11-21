//  tubebell-algo5.ck
//  4-operator FM instrument Tubular Bell example
//  Perry Cook, 2024 (after STK tubebell example)
Gain mixer[2] => NRev rev[2] => dac; // stereo
9 => int numBells; // round robin and stereo
TubeBell bell[numBells]; 
Pan2 pan[numBells];
for (int i; i < numBells; i++)  {
    bell[i] => pan[i] => mixer;
    bell[i].opADSR(0,0.01,0.4,0.0,0.04); // short envelopes for
    bell[i].opADSR(2,0.01,0.4,0.0,0.04); // more rapid articulation
    -1.0 + 0.25*i => pan[i].pan; // stereo bell spread
}
TubeBell bassBell => Pan2 bbPan => mixer; // occasional pedal tone
0.05 => rev[0].mix => rev[1].mix;
bell => Delay del => Pan2 dpan => rev; // cool echo
del => del; 
0.6 => del.gain;
0.2::second => dur T;
17*T => del.max => del.delay;

// Tubular Bells, Michael Oldfield (Excorcist Theme)
[64,72,64,74,64,71,72,64,71,64,69,64,71,64,67,69] @=> int pat1[];
[64,72,64,74,64,71,72,64,69,64,71,64,67,69] @=> int pat2[];

0 => int round;

<<< "FM Bell", "Sine waves yield inharmonic spectrum!!">>>;
<<< "Long Bell Ding!!", "110 Hz" >>>;
110 => bassBell.freq; // pedal A note (occasionally)
1 => bassBell.noteOn;
2*second => now;
<<< "Dong!!", "220 Hz" >>>;
220 => bassBell.freq; // pedal A note (occasionally)
1 => bassBell.noteOn;
2*second => now;
<<< "Dang!!", "330 Hz" >>>;
330 => bassBell.freq; // pedal A note (occasionally)
1 => bassBell.noteOn;
2*second => now;
110 => bassBell.freq; // pedal A note (occasionally)
1 => bassBell.noteOn;

<<< "Melody on shorter envelope bells", "" >>>;
while (1) {
    int pat[];
    if (maybe) {
        playPattern(pat1);
        <<< "Pattern 1", "" >>>;
    }
    else {
        playPattern(pat2);
        <<< "Pattern 2", "" >>>;
    }
}

fun void playPattern(int pat[])  {
    1 => int oct;
    if (maybe*maybe) {
        2 => oct; // occasional octave higher
        <<< "Octave higher", "" >>>;
    }
    for (0 => int i; i < pat.cap(); i++)  {
        Std.mtof(pat[i])*oct => bell[round].freq;
        if (i == 1 && maybe*maybe) {
            1 => bassBell.noteOn;
            <<< "Dong!", "" >>>;
        }
        1 => bell[round].noteOn;
        round++;
        if (round == numBells) 0 => round;
        T => now;
    }
}
    