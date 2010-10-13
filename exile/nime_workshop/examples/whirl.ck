sinosc s => dac;

0.0 => float t;
while( true )
{
    ( math.sin(t) + 1.0 ) * 10000.0 => s.sfreq;
    t + .004 => t;
    1::ms => now;
}
