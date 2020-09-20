// Listing 7.2 Playing the ChucK STK Brass PM UGen

// Playing a PM UG trumpet in a nice space
Brass trump => NRev rev => dac;
0.1 => rev.mix;

// notes for our "exercise"
[82,84,86,84,82,84,86,84,82,77,86,86,89] @=> int notes[];

// play through the exercise, except for the last note
0 => int i;

while (i < notes.cap()-1)
{
    0.8 => trump.startBlowing;
    Std.mtof(notes[i]) => trump.freq;
    0.1 :: second => now;
    1 => trump.stopBlowing;
    0.05 :: second => now;
    i++;
}

// add a little breath vibrato for last note
0.4 => trump.vibratoGain;
0.8 => trump.startBlowing;
Std.mtof(notes[i]) => trump.freq;
second => now;
1 => trump.stopBlowing;
second => now;
