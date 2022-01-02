// working with arrays of UGens
SndBuf buf => Pan2 pan => NRev reverb[2] => Gain master[2] => dac;
// set reveb mix
0.1 => reverb[0].mix => reverb[1].mix;
// load sound
"special:dope" => buf.read;

// infinite time loop
while( true )
{
    // randomize pan
    Math.random2(-1, 1) => pan.pan;
    // reset playback position
    0 => buf.pos;
    // advance time
    0.35::second => now;
}
