// demo3.ck
// super lame

// global gain
Gain g => dac;
// set gain
.5 => g.gain;

110.0 => float freq;
6 => int x;

// loop
while( x > 0 )
{
    // connect to gain
    SinOsc s => g;
    // change frequency
    freq => s.freq;
    freq * 2.0 => freq;
    // decrement x
    1 -=> x;

    // advance time by 1 second
    1::second => now;
    // disconnect the sinosc
    s =< g;
}
