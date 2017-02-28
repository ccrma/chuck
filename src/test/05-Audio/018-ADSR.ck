Noise n => ADSR e => dac;
50 => int steps;
30::second => dur duration;

0.5 => e.sustainLevel;
duration/steps/4 => e.attackTime;
duration/steps/4 => e.decayTime;
duration/steps/4 => e.releaseTime;
Std.srand(0);

for(int i; i < steps; i++)
{
    e.keyOn();
    duration/steps/2 => now;
    e.keyOff();
    duration/steps/2 => now;
}