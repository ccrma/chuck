SinOsc s => Chorus c => dac;
30::second => dur duration;
1000 => int steps;

0.5 => c.mix;
0.5 => c.modDepth;

for(int i; i < steps; i++)
{
    i => c.modFreq;
    duration/steps => now;
}
