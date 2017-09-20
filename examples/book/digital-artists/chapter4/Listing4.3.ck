// Listing 4.3 Playing a sound file backward

// Playing sounds in reverse
// by ChucK Programmer 4102, yluJ  (reverse date!)
SndBuf mySound => dac;

// (1) Assembles file path/name and read file into SndBuf.
me.dir()+"/audio/hihat_04.wav" => mySound.read;

// (2) Asks the sound how long it is (in samples).
mySound.samples() => int numSamples;

// play sound once forward
0 => mySound.pos;
numSamples :: samp => now;  // (3) Lets it play for that long.

// and once backward
numSamples => mySound.pos;  // (4) Sets position to end of buffer.
-1.0 => mySound.rate;       // (5) Sets it to play backward
numSamples :: samp => now;  // Play for whole duration, but backward.

