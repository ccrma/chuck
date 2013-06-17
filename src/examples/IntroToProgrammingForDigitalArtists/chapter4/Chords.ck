// Sound Network
SinOsc chord[3];
for (0 => int i; i < chord.cap(); i++)
{
    chord[i] => dac; // use array to chuck Unit Generator to DAC
    1.0/chord.cap() => chord[i].gain; // Set Volume so no Peaking
}

// Function
fun void playChord(int root, string quality, float length)
{
    // function will make Major or Minor Chords
    
    //root
    Std.mtof(root) => chord[0].freq;
    
    // third
    if (quality == "major")
    {
        Std.mtof(root+4) => chord[1].freq; // major interval
    }
    else if (qaulity == "minor")
    {
        Std.mtof(root+3) => chord[1].freq; // minor interval
    }
    
    // fifth
    Std.mtof(root+7) => chord[2].freq;
    
    length::ms => now;
}

// Main Program
while (true)
{
    // Procedurally play through 3 chords: minor, major, minor
    playChord(Std.rand2(60,72), "minor", 250);
    playChord(Std.rand2(60,72), "major", 250);
    playChord(Std.rand2(60,72), "minor", 500);
}


