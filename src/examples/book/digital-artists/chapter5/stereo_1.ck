// sound chain
SndBuf2 mySound => dac;

// read the file into the buffer
me.dir()  + "/audio/stereo_fx_01.wav" => string filename;
// open up soundfile
filename => mySound.read; 

// infinite loop
while( true )
{
    Math.random2f(.6,1.0) => mySound.gain; // set volume
    Math.random2f(.2, 1.8) => mySound.rate; // set rate
    0 => mySound.pos; // sets playhead position
    5000::ms => now;
}

