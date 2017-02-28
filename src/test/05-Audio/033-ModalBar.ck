ModalBar ug => dac;
30::second => dur duration;
4 => int presets;

1000 => ug.freq;

for (int i; i < presets; i++)
{
    i => ug.preset;
    1.0 => ug.strike;
    duration/presets => now;
}
