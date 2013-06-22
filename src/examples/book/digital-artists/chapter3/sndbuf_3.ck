// sound chain
SndBuf mySound => Pan2 p => dac;

// read the file into the buffer
me.dir()  + "/audio/snare_01.wav" => string filename;
// open soundfile
filename => mySound.read; 
// find number of samples 
mySound.samples() => int numSamples; 

// infinite loop
while( true )
{
    numSamples => mySound.pos; // sets playhead position
    -1.0 => mySound.rate; // set rate - negative for Reverse
    1::second => now; // advance time
}


