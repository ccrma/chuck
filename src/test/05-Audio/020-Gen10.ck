Phasor drive => Gen10 g10 => dac;

30::second => dur duration;
1000 => int max;


g10.coefs( [1., 0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1] );

for (0 => int i; i < max; i++)
{
    i => drive.freq;
    duration/max => now;
}
