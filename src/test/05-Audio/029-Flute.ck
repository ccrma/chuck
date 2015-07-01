Flute ug => dac;
30::second => dur duration;
100 => int steps;

1000 => ug.freq;
Std.srand(0);

for (int i; i < steps; i++)
{
    ug.noteOn(1.0);
    ug.startBlowing(1.0);
    duration/steps/2 => now;
    ug.stopBlowing(1.0);
    duration/steps/2 => now;
    ug.noteOff(1.0);
}

