// Listing 7.11 Bowing a rigid bar using the BandedWG UGen

// Test some Banded WaveGuide parameters
BandedWG bwg => dac;

// set to model Tibetan prayer bowl
3 => bwg.preset;

// whack the "bowl" to get it started
1 => bwg.noteOn;

// bring the bow to the surface
1 => bwg.startBowing;

// then bow it with increasing pressure
for (0 => int i; i <= 7; i++)
{
    <<< "bowPressure=", i*0.1 => bwg.bowPressure >>>;
    second => now;
}

// then bow it with decreasing pressure
for (6 => int i; i > 0; i--)
{
    <<< "bowPressure=", i*0.1 => bwg.bowPressure >>>;
    second => now;
}
