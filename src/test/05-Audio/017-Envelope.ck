Noise n => Envelope e => dac;
50 => int steps;
30::second => dur duration;

duration/steps/4 => e.duration;
Std.srand(0);

for(int i; i < steps; i++)
{
    e.keyOn();
    duration/steps/2 => now;
    e.keyOff();
    duration/steps/2 => now;
}