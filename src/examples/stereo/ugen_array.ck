SndBuf buf => Pan2 pan => NRev reverb[2] => Gain master[2] => dac;

0.1 => reverb[0].mix => reverb[1].mix;

"special:dope" => buf.read;

while(true)
{
    Math.random2(-1, 1) => pan.pan;
    0 => buf.pos;
    
    0.25::second => now;
}
