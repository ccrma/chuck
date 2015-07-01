Phasor s => dac;
1000 => int max;
30::second => dur duration;
for(int i; i < max; i++)
{
    i => s.freq;
    duration/max => now;
}
