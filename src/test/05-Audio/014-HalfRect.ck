Impulse s => HalfRect r => dac;
30::second => dur duration;
100 => int steps;

for(int i; i < steps; i++)
{
    -s.next() => s.next;
    duration/steps => now;
}
