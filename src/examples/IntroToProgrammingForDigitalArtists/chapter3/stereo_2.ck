// create stereo SndBuf
SndBuf2 mySound;
// two gains to control stereo balance 
Gain balance[2];  

// connect left channel
mySound.chan(0) => balance[0] => dac.left;
// connect right channel
mySound.chan(1) => balance[1] => dac.right;

// read the file into the buffer
me.dir()  + "/audio/stereo_fx_05.wav" => string filename;
// open up soundfile
filename => mySound.read; 

// infinite loop 
while( true )
{
    Math.random2f(.6,1.0) => mySound.gain; // set volume
    Math.random2f(.2, 1.8) => mySound.rate; // set rate
    Math.random2f(-1.0, 1.0) => float bal; // balance val
    1-(bal+1)/2.0 => balance[0].gain; // map to gain [1,0]
    (bal+1)/2.0 => balance[1].gain; // map to gain [0,1]  
    0 => mySound.pos; // sets playhead position
    5000::ms => now; // advance time
}
