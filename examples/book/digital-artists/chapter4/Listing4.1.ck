// Listing 4.1 Loading and playing sound files with SndBuf

// Using SndBuf to play a sound file
// by ChucK Programmer, December 2050
SndBuf mySound => dac;

// get file path
me.dir() => string path;    // (1) Gets current working directory.

// sound file we want to play
"/audio/snare_01.wav" => string filename; 

// + sign connects strings together!
path+filename => filename;  // (2) builds a full path/name to a sound file.

// tell SndBuf to read this file
filename => mySound.read;   // (3) ChucKing a string to the .read member of 
                            //     SndBuf causes it to load that file

// set gain
0.5 => mySound.gain;

// play sound from the beginning
0 => mySound.pos;      // ChucKing a number to the .pos (for
                       // position) member of SndBuf causes it to
                       // start playing from that position in the
                       // array (in this case, from the beginning).

// advance time so we can hear it
second => now;
