// Listing 9.21 MandoScore.ck makes and plays your new MandoPlayer
// let's try all this out!! Make a MandoPlayer object
MandoPlayer m;  // (1) Makes a smart MandoPlayer object

// and declare some data for chording and strumming
["G","C","G","D","D","D","D","G"] @=> string chords[]; // (2) Array of string chord names
[0.4,0.4,0.4,0.1,0.1,0.1,0.1,0.01] @=> float durs[];
[79,81,83] @=> int strums[];                           // (3) Array of things to strum

// counter to iterate through the arrays
0 => int i;

// roll the basic chords, reading through the arrays
while (i < chords.cap()) {        // (4) ...through whole chord name array
    m.roll(chords[i],durs[i] :: second); // (5) Rolls each chord
    i++;
}

// now strum a few notes
0 => i;
while (i < strums.cap()) {          // (6) Strums through the array of strums
    m.strum(strums[i++], 1.0 :: second);
}

// then end up with a big open G chord
m.damp(1.0);                 // (7) Lets strings ring a long time
m.roll("G", 0.02 :: second); // (8) Rolls a chord
2.0 :: second => now;

// damp it to silence, letting it ring a little
m.damp(0.01);                // (9) Damps strings at the end
1.0 :: second => now;
