SinOsc s => Pan2 p => dac;

30::second => dur duration;
1000 => int steps;

1000 => s.freq;

for (0 => int i; i < steps; i++)
{
    (2.0 / steps * i) - 1.0 => p.pan;
    duration / steps => now;
}
