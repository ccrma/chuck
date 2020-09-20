// Listing 1.8 Listing 1.8 Playing notes with integer variables

/* Sine Music with integer variables
by ChucK Programmer
January 2025 */

SinOsc s => dac;

220 => int myPitch;    // (1) Declares and initializes an integer variable called myPitch

1 => int onGain;       // (2) Declares and initializes two integers for
0 => int offGain;      //     controlling gain

// Play one note       // (3) Plays a note using your new integer variables
myPitch => s.freq;
onGain => s.gain;
0.3 :: second => now;

offGain => s.gain;     // (4) Turns off the sound of your oscillator
0.3 :: second => now;  //     to separate the notes

1 *=> myPitch;         // (5) Multiplies your pitch by 2, in place

// Play another note, with a higher pitch
myPitch => s.freq;     // (6) Sets frequency and turns on
onGain => s.gain;      //     oscillator, to start your 2nd note
0.3 :: second => now;

offGain => s.gain;     // (7) Turns off the sound of your oscillator 
0.3 :: second => now;  //     again to end the 2nd note


