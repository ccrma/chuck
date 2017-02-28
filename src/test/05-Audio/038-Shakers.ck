Shakers wg => dac;
30::second => dur duration;
23 => int presets;

Std.srand(0);

for (int i; i < presets; i++)
{
    i => wg.preset;
    1.0 => wg.noteOn;
    duration/presets => now;
    1.0 => wg.noteOff;
}

