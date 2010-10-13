// using SinOsc.period (alternative to .freq)

// sinosc
SinOsc lfo => dac;
// set period
1::second => lfo.period;

// infinite time loop
while( true )
{
    <<< lfo.last(), "" >>>;
    50::ms => now;
}
