Gain g;
SinOsc s;
SndBuf b;
TriOsc t;
PitShift p;

true => g.buffered 
     => s.buffered 
     => b.buffered 
     => t.buffered
     => p.buffered 
     => dac.buffered;

if( g.buffered() &&
    s.buffered() &&
    b.buffered() &&
    t.buffered() &&
    p.buffered() &&
    dac.buffered() )
{
    <<< "success" >>>;
}