// Listing 4.2 Using a loop to repeatedly play a sound file

// Play a sound file repeatedly in a loop
// by ChucK Programmer, January 12, 2017
SndBuf mySound => Pan2 pan => dac;   // Connects a SndBuf through a
                                     // Pan2 panner object to the DAC

// get file path and load file all in one line!
me.dir()+"/audio/cowbell_01.wav" => mySound.read;

// play our sound over and again in an infinite loop
while (true) 
{
    // random gain, rate (pitch), and pan each time
    Math.random2f(0.1,1.0) => mySound.gain; // (1) Random gain for sound file
    Math.random2f(-1.0,1.0) => pan.pan;     // (2) Random pan position
    Math.random2f(0.2,1.8) => mySound.rate; // (3) Random rate (speed and pitch)

    // (re)start the sound by setting position to 0
    0 => mySound.pos;     // (4) Sets pos to zero to start it playing

    // advance time so we can hear it
    500.0 :: ms => now;   // (5) Hangs out a bit while it plays
}
