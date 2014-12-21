// Listing 7.4 Playing strings of different stiffness using the StifKarp UGen

StifKarp skarp => dac;    // (1) Stiff string model
while (1)
{
                          // (2) .stretch method controls stiffness
    Math.random2f(0.3, 1.0) => skarp.stretch;
    1 => skarp.noteOn;
    0.3 :: second => now;
}
