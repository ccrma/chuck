Step s => dac;
30::second => dur duration;
100 => int steps;

1.0 => s.next;

for(int i; i < steps; i++)
{
    -s.next() => s.next;
    duration/steps => now;
}
