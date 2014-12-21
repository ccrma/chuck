// Listing 7.13 Experimenting with the parameters of one particular shaker preset

// Mess with the parameters of the (1) "Quarters in coffee mug" preset
Shakers shak => dac;
17 => shak.preset;

// then fiddle around with #objects and damping
while (1) {
    Math.random2f(0.0,128.0) => shak.objects; // (2) Random number of quarters
    Math.random2f(0.0,1.0) => shak.decay;     // (3) Random amount of damping (decay time)
    <<< "#Objects=", shak.objects(), "damp=", shak.decay() >>>;
    1.0 => shak.energy;
    1.0 :: second => now;
}


