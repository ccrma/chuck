// Listing 7.1 Playing the ChucK STK Clarinet physical model unit generator

// STK Physical Clarinet Model
Clarinet clar => dac;

0.2 => float blow;             // initial blowing pressure

while ((0.05 +=> blow) < 1.0)  // gradually increases from there
{
    blow => clar.startBlowing; // alternately blowing
    0.2 :: second => now;

    1 => clar.stopBlowing;     // and not blowing
    0.2 :: second => now;
}

