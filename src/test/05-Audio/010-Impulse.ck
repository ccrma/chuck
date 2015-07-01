Impulse s => dac;
30::second => dur duration;
100 => int steps;

for(int i; i < steps; i++)
{
    i%2*2-1 => s.next;
    duration/steps => now;
}
