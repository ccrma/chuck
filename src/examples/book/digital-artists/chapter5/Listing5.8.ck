// Listing 5.8 Main program uses swell to expressively play a melody

// swell function, operates on any type of UGen
fun void swell(UGen osc, float begin, float end, float step)
{                                  // (1) swell function definition
    float val;

    // swell up volume
    for (begin => val; val < end; step +=> val)  // (2) for loop to ramp up volume
    {
        val => osc.gain;
        0.01 :: second => now;
    }

    // swell down volume
    while (val > begin)                          // (3) while loop to ramp it back down
    {
        val => osc.gain;
        step -=> val;
        0.01:: second => now;
    }
}

// Main program
// our sound "patch"
TriOsc tri => dac;            // (1) Triangle oscillator to test swell function

// global (2) Array of notes to play
[60,62,63,65,63,64,65,58,57,56] @=> int notes[];

// Swell through all notes
for (0 => int i; i < notes.cap(); i++) { // (3)  Iterates through notes array
    Std.mtof(notes[i]) => tri.freq;      // (4) Sets frequency
    swell(tri,0.2,1.0,0.01);             // (5) Calls swell function
}
