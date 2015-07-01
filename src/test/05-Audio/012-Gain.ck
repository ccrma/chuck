Gain g => dac;
SinOsc s => g;
30::second => dur duration;
1000 => int steps;

1000 => s.freq;

for (0 => int i; i < steps; i++)
{
    1.0 / steps * i => g.gain;
    duration / steps => now;
}
