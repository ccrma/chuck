// sound file to load; me.dir() returns location of this file
me.dir() + "../data/snare.wav" => string filename;
// if there is argument, use it as the filename
if( me.args() ) me.arg(0) => filename;

// the patch
SndBuf buf(filename) => dac;
// can also load the file this way
// filename => buf.read;

// check if file successfully loaded
if( !buf.ready() ) me.exit();

// time loop
while( true )
{
    // set playback position to beginning
    0 => buf.pos;
    // randomize gain
    Math.random2f(.25,.5) => buf.gain;
    // randomize rate
    Math.random2f(.5,1.5) => buf.rate;
    // advance time
    100::ms => now;
}
