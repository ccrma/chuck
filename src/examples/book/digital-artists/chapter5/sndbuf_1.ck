// sound chain
SndBuf mySound => dac;

// directory of this file
me.dir() => string path;
// read the file into the buffer
"/audio/snare_01.wav" => string filename;
// put together filename with path
path + filename => filename;
// open up soundfile
filename => mySound.read; 

// simple control
.5 => mySound.gain; // set volume
0 => mySound.pos; // sets playhead position

// advance time 
1::second => now;
