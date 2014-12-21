// Listing 7.17 Using ChucK's VoicForm formant voice UGen

// Simple singing example using the VoicForm UG
VoicForm singer => dac;
0.05 => singer.vibratoGain;

[40,42,44,45,47,45,44,42] @=> int scale[];

// Start singing our first note
Std.mtof(scale[0]) => singer.freq;
11 => singer.phonemeNum;          // (1) Selects vowel by number
1 => singer.noteOn;

// sing through whole scale
0 => int note;
while (note < scale.cap()){
    dipth("lll","ahh",scale[note]); // (2) Sings a diphthong (transition between two vowels)
    1 +=> note;
}

// last note
dipth("lll","ahh",scale[0]);
0.15 => singer.vibratoGain;
2.0 :: second => now;
1 => singer.noteOff;
0.2 :: second => now;

fun void dipth(string phon0, string phon1, int note) {
    phon0 => singer.phoneme;
    0.1 :: second => now;
    phon1 => singer.phoneme;
    Std.mtof(note) => singer.freq;
    0.4 :: second => now;
}
