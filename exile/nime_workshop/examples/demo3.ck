// set global gain
.5 => dac.gain;

110.0 => float freq;
6 => int x;

while( x > 0 )
{
    sinosc s => dac;
    freq => s.freq;
    freq * 2.0 => freq;
    x - 1 => x;    

    1::second => now;
    s =< dac;
}
