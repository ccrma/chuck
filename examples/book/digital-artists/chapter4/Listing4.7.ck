// Listing 4.7 Stereo panning with stereo sound files using SndBuf2

// Loading and panning stereo sound files
// by ChucK Programmer, October, 2023

// declare and load up a stereo file
// Make a stereo SndBuf2 and
SndBuf2 myStereoSound;  
me.dir()+"/audio/stereo_fx_03.wav" => myStereoSound.read; // load a stereo sound file.

// We'll use these for Stereo Panning
Gain bal[2];  // (2) Makes a Gain UGen array for stereo volume control.

// connect everything up in stereo
myStereoSound.chan(0) => bal[0] => dac.left;  // (3) Connects left to left,
myStereoSound.chan(1) => bal[1] => dac.right; //     right to right.

// set our soundfile to repeat forever
1 => myStereoSound.loop;                      // (4) Repeats automatically.

while (true) {
    // pick a random playback rate and a random panning
    Math.random2f(0.2,1.8) => myStereoSound.rate; // (5) Sets a random rate (pitch and time).

    Math.random2f(-1.0,1.0) => float balance;     // (6) Sets a random balance (pan).

    // turn balance into left/right gain between 0 and 1.0
    (balance+1)/2.0 => float rightGain;           // (7) Implements the stereo balance control.
    1.0 - rightGain => float leftGain;
    leftGain => bal[0].gain;
    rightGain => bal[1].gain;

    0.3 :: second => now;
}
