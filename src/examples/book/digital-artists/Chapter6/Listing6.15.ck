// Listing 6.15 Musical fun with a resonant filter and three delay lines

// Fun with UGens! By UG dude, Oct 14, 2020
// Impulse-excited resonant filter drives
// three delay lines, fed back to themselves
Impulse imp => ResonZ rez => Gain input => dac;
      //  (1) Direct path of resonant filtered impulse.

100 => rez.Q;
100 => rez.gain;
1.0 => input.gain;

// We can make arrays of UGens, too
Delay del[3];  // (2) Array of three delay lines.

// Let's have some stereo   (3) Left, right, center delay outputs.
input => del[0] => dac.left;
input => del[1] => dac;
input => del[2] => dac.right;

// (4) Set up all the delay lines
for (0 => int i; i < 3; i++) {
    del[i] => del[i];
    0.6 => del[i].gain;
              // (5) Each delay time is different but related. 
    (0.8 + i*0.3) :: second => del[i].max => del[i].delay;
}

// Define note array for our song
//              (6) Array of notes that you'll draw from. 
[60, 64, 65, 67, 70, 72] @=> int notes[];
notes.cap() - 1 => int numNotes;

// Let the fun begin! (and continue forever)
while (1) {
    // (7) Play a random note (resonant filter frequency).
    Std.mtof(notes[Math.random2(0,numNotes)]) => rez.freq;
    1.0 => imp.next;   // (8) Fires impulse (output 1 on next sample).
    0.4 :: second => now;
}
