// Listing 1.11 Twinkle with dur variables

/* Sine Music using dur variables
by ChucK Programmer
January 2025 */

SinOsc s => dac;

220.0 => float twinkle;
0.55 :: second => dur onDur;  // (1) Defines note durations as variables
0.05 :: second => dur offDur;

1 => int onGain;
0 => int offGain;

// Play one note
twinkle => s.freq;
onGain => s.gain;
onDur => now;                 // (2) Waits while note sounds...

offGain => s.gain;
offDur => now;               // (3) ...then waits for space between notes

1.5 *=> twinkle;             // (4) Next note frequency

// Play other note of 2nd "twinkle"
twinkle => s.freq;           // (5) Sets it and plays another note
onGain => s.gain;
onDur => now;
offGain => s.gain;
offDur => now;
