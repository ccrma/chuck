// the patch 
sndbuf buf => dac;
// load the file
"data/snare.wav" => buf.read;
// set the playback rate (1.0 is default)
1.0 => buf.play; 

// time loop
while( true )
{
    0 => buf.pos;
    std.rand2f(.2,.9) => buf.gain;
    100::ms => now;
}
