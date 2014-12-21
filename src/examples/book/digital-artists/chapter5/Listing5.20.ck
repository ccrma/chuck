// Listing 5.19 Playing chords on an array of SinOsc UGens, using a function

// Sound Network
SinOsc chord[3];               // (1) Three oscillators for a chord

for (0 => int i; i < chord.cap(); i++)
{
    // connect each element of our array to dac
    chord[i] => dac;           // (2) Connects them all to the dac...

    // adjust gain so we don't overload
    1.0/chord.cap() => chord[i].gain; // (3) ...and sets their gains so you don't overload.
}

fun void playChord(int root, string quality, dur howLong)
{
    // set root of chord
    Std.mtof(root) => chord[0].freq;        // (4) Root of chord.

    // set fifth of chord
    Std.mtof(root+7) => chord[2].freq;      // (5) Fifth of chord.

    // third sets quality, major or minor
    if (quality == "major")
    {
        Std.mtof(root+4) => chord[1].freq;  // (6) Major chord.
    }
    else if (quality == "minor") {
        Std.mtof(root+3) => chord[1].freq;  // (7) Minor chord.
    }
    else
    {
        <<< "You must specify major or minor!!" >>>;
    }
    howLong => now;
}

// Listing 5.20 Using playChord

// Main program: now let's use playChord!!
while (true)
{
    playChord(Std.rand2(70,82), "minor", second/2); // (1) Plays a minor chord on a random note
    playChord(60, "minor", second/2);      // (2) Plays a C minor chord
    playChord(67, "major", second/2);      // (3) Plays a G major chord    
}
