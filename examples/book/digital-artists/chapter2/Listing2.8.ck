// Listing 2.8 Two-part random walk music with panning

// 2-part Random Music with Panning
// by ChucK Team, September 25, 2020
// two oscillators, melody and harmony
SinOsc s => Pan2 mpan => dac;  // (1) SinOsc through Pan2 so it canmove around
TriOsc t => dac;               // (2) TriOsc fixed at center location

// we will use these to separate notes later
0.5 => t.gain;                 // (3)  Float variables to control your note gains
0.5 => float onGain;
0.0 => float offGain;
72 => int melodyNote;          // (4) Int variable to control your melody

while (true)
{
    // set melody pitch somewhat randomly, with limits
    Math.random2(-3,3) +=> melodyNote;  // (5) Randomly changes melody up, down, or not

    if (melodyNote < 60)       // (6) Lower limit on melody
    {
        60 => melodyNote;
    }
    if (melodyNote > 84)       // (7) Upper limit on melody
    {
        84 => melodyNote;
    }

    Std.mtof(melodyNote) => s.freq;  // (8) Sets solo SinOsc pitch

    // melody has a random pan for each note
    Math.random2f(-1.0,1.0) => mpan.pan;

    // On a "dice roll," change harmony note
    if (Math.random2(1,6) == 1)
    {                               // (9) Randomly sticks TriOsc on a pitch
        Std.mtof(melodyNote-12) => t.freq; 
    }

    // Pick one of three random durations
    Math.random2(1,3)*0.2 => float myDur;

    // note on time is 80% of duration
    onGain => s.gain;
    (myDur*0.8)::second => now;

    // space between notes is 20% of array duration
    offGain => s.gain;
    (myDur*0.2)::second => now;
}

