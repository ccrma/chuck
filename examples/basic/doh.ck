// DOH!!!

// the patch 
SndBuf buf => dac;
// load the file ("special:" files are built into chuck)
"special:dope" => buf.read;

// time loop
while( true )
{
    // set playback position to beginning
    0 => buf.pos;
    // randomize playback rate
    Math.random2f(.75,1.25) => buf.rate;
    // randomize time
    500::ms => now;
}
