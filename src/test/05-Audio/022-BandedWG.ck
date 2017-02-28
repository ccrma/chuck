BandedWG ug => dac;
30::second => dur duration;
4 => int presets;

1000 => ug.freq;
Std.srand(0);

for (int i; i < presets; i++)
{
   i => ug.preset;

   1.0 => ug.pluck;
   duration/presets => now;
}

