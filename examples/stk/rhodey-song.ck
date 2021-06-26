// name: rhodey-song.ck
// desc: Rhodey 4-Operator FM (TX81Z Algorithm 5) Demo
//       An homage to Gil Scott Heron's "Winter in America"
//
// author: Perry R. Cook
// date: June 2021
// note: needs chuck 1.4.1.0 or above

// to learn more about Rhodey, uncomment this:
// Rhodey.help();

Rhodey r[5];
NRev rev[2] => dac; 0.05 => rev[0].mix; 0.05 => rev[1].mix;
Rhodey solo => rev;
Pan2 p[2]; p[0] => rev[0]; p[1] => rev[1];
Delay d[2]; rev[0] => d[0] => d[1] => rev[1]; 
0.7 => d[0].gain => d[1].gain;
second => d[0].max => d[0].delay => d[1].max => d[1].delay;
SinOsc panner => blackhole;
1 => panner.freq;
r[0] => p[0]; r[1] => p[0]; 
r[2] => p[1]; r[3] => p[1]; r[4] => p[0]; r[4] => p[1];
for (int i; i < r.cap(); i++) {
    1 => r[i].lfoSpeed;
    0.0 => r[i].lfoDepth;
    r[i].opAM(0,0.4);
    r[i].opAM(2,0.4);
    r[i].opADSR(0, 0.001, 3.50, 0.0, 0.04);
    r[i].opADSR(2, 0.001, 3.00, 0.0, 0.04);
}

// spork panner on its own shred   
spork ~ doPan();

//  Some chords.  We don't use all here
[41, 56, 60, 63, 67] @=> int Fmi9[];
[36, 55, 58, 62, 65] @=> int BfMajC[];
[34, 56, 60, 63, 67] @=> int AfMaj7Bf[];
[43, 53, 58, 62, 65] @=> int Gmi7[];
[62, 66, 69, 74] @=> int DMaj[];
[59, 62, 66, 68] @=> int Bmi6[];
[55, 59, 62, 66] @=> int GMaj7[];
[59, 62, 66, 71] @=> int Bmi[];
[62, 66, 69, 73] @=> int D7[];
[62, 66, 69, 74] @=> int DFs[];

second/2 => dur Q; Q/2 => dur E; E/2 => dur S;

while( true )
{
    Std.mtof(72) => solo.freq; 0.8 => solo.noteOn; 2.5*Q => now;
    Std.mtof(70) => solo.freq; 0.9 => solo.noteOn; E => now;
    Std.mtof(68) => solo.freq; 0.7 => solo.noteOn; Q+S => now;
    spork ~ rollChord(Fmi9, 0.9); 4*second => now;
    Std.mtof(65) => solo.freq; 0.6 => solo.noteOn; E => now;
    Std.mtof(68) => solo.freq; 0.9 => solo.noteOn; 1.5*E => now;
    Std.mtof(65) => solo.freq; 0.7 => solo.noteOn; E/2 => now;
    Std.mtof(70) => solo.freq; 0.9 => solo.noteOn; 
    spork ~ rollChord(BfMajC, 0.9);
    Q+S => now;
    Std.mtof(63) => solo.freq; 0.6 => solo.noteOn; E => now;
    Std.mtof(65) => solo.freq; 0.7 => solo.noteOn; Q+S => now;
    4*second => now;
    Std.mtof(65) => solo.freq; 0.6 => solo.noteOn; E => now;
    Std.mtof(68) => solo.freq; 0.9 => solo.noteOn; E => now;
    Std.mtof(70) => solo.freq; 0.7 => solo.noteOn; E => now;
    Std.mtof(72) => solo.freq; 0.9 => solo.noteOn; 
    spork ~ rollChord(Fmi9, 0.9);
    4*second => now;
    Std.mtof(65) => solo.freq; 0.6 => solo.noteOn; E => now;
    Std.mtof(68) => solo.freq; 0.9 => solo.noteOn; E => now;
    Std.mtof(65) => solo.freq; 0.7 => solo.noteOn; E => now;
    Std.mtof(70) => solo.freq; 0.9 => solo.noteOn; 
    spork ~ rollChord(BfMajC, 0.9); 4*second => now;

    Std.mtof(65) => solo.freq; 0.6 => solo.noteOn; E => now;
    Std.mtof(68) => solo.freq; 0.9 => solo.noteOn; E => now;
    Std.mtof(65) => solo.freq; 0.7 => solo.noteOn; E => now;
    Std.mtof(70) => solo.freq; 0.7 => solo.noteOn; E => now;
    Std.mtof(72) => solo.freq; 0.9 => solo.noteOn; 
    spork ~ rollChord(Fmi9, 0.9); Q => now;
    Std.mtof(75) => solo.freq; 0.9 => solo.noteOn; E/2 => now;
    Std.mtof(77) => solo.freq; 0.9 => solo.noteOn; E/2 => now;
    Std.mtof(80) => solo.freq; 1.0 => solo.noteOn; 
    4*second => now;
    Std.mtof(65) => solo.freq; 0.6 => solo.noteOn; E => now;
    Std.mtof(68) => solo.freq; 0.9 => solo.noteOn; 1.5*E => now;
    Std.mtof(77) => solo.freq; 0.7 => solo.noteOn; E/2 => now;
    Std.mtof(82) => solo.freq; 0.9 => solo.noteOn; 
    spork ~ rollChord(BfMajC, 0.9);
    4*second => now;

    Std.mtof(72) => solo.freq; 0.8 => solo.noteOn; Q => now;
    Std.mtof(75) => solo.freq; 0.9 => solo.noteOn; E => now;
    Std.mtof(72) => solo.freq; 0.8 => solo.noteOn; E => now;
    Std.mtof(75) => solo.freq; 0.9 => solo.noteOn; E/2 => now;
    Std.mtof(72) => solo.freq; 0.8 => solo.noteOn; E => now;
    spork ~ rollChord(AfMaj7Bf, 0.9);
    4*second => now;
    
    Std.mtof(65) => solo.freq; 0.6 => solo.noteOn; E => now;
    Std.mtof(63) => solo.freq; 0.6 => solo.noteOn; E => now;
    Std.mtof(53) => solo.freq; 1.0 => solo.noteOn;
    spork ~ rollChord(Gmi7, 0.9);
    6*second => now;
}


fun void chord(int chord[], float vel) {
    for (int i; i < chord.cap(); i++) {
        Std.mtof(chord[i]) => r[i].freq;
        vel => r[i].noteOn;
    }
}

fun void rollChord(int chord[], float vel) {
    for (int i; i < chord.cap(); i++) {
        Std.mtof(chord[i]) => r[i].freq;
        vel => r[i].noteOn;
        Math.random2f(0.01,0.08)::second => now;
    }
}

fun void doPan() {
    while( true ) {
        1.0 - panner.last() => float temp;
        temp => p[0].pan;
        1.0 - temp => p[1].pan;
        ms => now;
    }
}
