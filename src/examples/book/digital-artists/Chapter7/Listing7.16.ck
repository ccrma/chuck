// Listing 7.16 Building a simple formant (resonant filter) voice model

// Simple singing voice model
// by Enrico C., August 1921
SawOsc folds => ResonZ formant1 => dac; // (1) Sawtooth models vocal folds through formant filter
folds => ResonZ formant2 => dac;        // (2) 2nd formant (vocal tract resonance)
folds => ResonZ formant3 => dac;        // (3) 3rd formant

// hook up a modulation (vibrato) oscillator
SinOsc vibrato => folds;                // (4) Adds vibrato to voice source
6.0 => vibrato.freq;
1.5 => vibrato.gain;
2 => folds.sync;

// Set up filter resonance amounts
20 => formant1.Q => formant2.Q => formant3.Q; // (5) Sets resonance of all three formant filters

// Sing forever, randomly
while (1) {                      // (6) Sings a random "song" forever
                                 // (7) Random "vowel" (formant frequencies)
    Math.random2f(200,750) => formant1.freq;
    Math.random2f(900,2300) => formant2.freq;
    Math.random2f(2400,3600) => formant3.freq;
    if (Math.random2(0,3) == 0)
    {                            // (8) Random voice pitch
        Math.random2f(60.0,200.0) => folds.freq;
    }
    Math.random2f(0.2,0.5) :: second => now;
}
