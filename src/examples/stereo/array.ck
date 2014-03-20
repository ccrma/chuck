// dope storm
// using arrays as UGen busses
// author: Spencer Salazar - spencer@ccrma.stanford.edu
// date: May 2013

SndBuf buf[32] => Pan2 pan[32] => NRev reverb[2] => dac;

0.1 => reverb[0].mix;
0.11 => reverb[1].mix;

for(int i; i < buf.cap(); i++)
{
    // load dope sound
    "special:dope" => buf[i].read;
    // set to end
    buf[i].samples() => buf[i].pos;
}

0 => int i;

while(true)
{
    //Math.pow(2, Std.rand2f(-1.5, 1.5)) => buf[i%buf.cap()].rate;
    Std.rand2f(-0.5,0.5) => pan[i%pan.cap()].pan;
    0 => buf[i%buf.cap()].pos;
    
    Std.rand2f(0.075, 0.125)::second => now;
    //0.5::second => now;
    
    i++;
}
