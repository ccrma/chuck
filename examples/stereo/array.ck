// dope storm
// using arrays as UGen busses
// author: Spencer Salazar - spencer@ccrma.stanford.edu
// date: May 2013

// patch mixing arrays of mono and stereo UGens
SndBuf buf[32] => Pan2 pan[32] => NRev reverb[2] => dac;

// set reverb mix
0.1 => reverb[0].mix;
0.11 => reverb[1].mix;

// load sound
for( int i; i < buf.size(); i++ )
{
    // load dope sound
    "special:dope" => buf[i].read;
    // set to end
    buf[i].samples() => buf[i].pos;
}

// index
0 => int i;

while(true)
{
    // randomize pan
    Math.random2f(-0.5,0.5) => pan[i%pan.size()].pan;
    // Math.pow(2, Math.random2f(-1.5, 1.5)) => buf[i%buf.size()].rate;

    // reset playback position for a buf
    0 => buf[i%buf.size()].pos;

    // randomize time
    Math.random2f(0.075, 0.125)::second => now;
    //0.5::second => now;

    // increment    
    i++;
}
