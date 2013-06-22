// sound chain
SndBuf mySound => Pan2 p => dac;


// read the file into the buffer
"/audio/snare_01.wav" => string filename;
// put together filename with path
path + filename => filename;
// open up soundfile
filename => mySound.read; 

// infinite loop
while( true )
{
    Math.random2f(.2,.8) => mySound.gain; // set volume
    Math.random2f(.2, 1.8) => mySound.rate; // set rate
    Math.random2f(-1.0, 1.0) => p.pan; // set panning
    0 => mySound.pos; // sets playhead position
    100::ms => now; // advance time
}


