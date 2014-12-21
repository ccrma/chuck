// Listing 7.12 Testing all the presets for the Shakers UG

// Fun with particles, the Shakers instrument
Shakers shak => dac;

// Run through all the presets in order
for (0 => int i; i < 23; i++) {
    i => shak.preset;
    1 => shak.noteOn;
    1.0 :: second => now;
}
