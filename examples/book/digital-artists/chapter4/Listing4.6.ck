// Listing 4.6 Loading and playing stereo sound files using SndBuf2

// Loading and playing stereo sound files
// by ChucK Programmer, September, 2023
SndBuf2 myStereoSound => dac;   // Makes a stereo SndBuf2
                                // and connects it to the dac
                                // (stereo is automatic!).

// load up a stereo file using .read
me.dir()+"/audio/stereo_fx_01.wav" => myStereoSound.read;

// and let it play for the right amount of time
myStereoSound.length() => now;  // (1) New function/method!
                                //    .length returns the exact
                                //     duration of the sound file.


