// Listing 8.3 Controlling different aspects of a single global object using concurrency

// sound chain
ModalBar modal => NRev reverb => dac;  // (1) Modal bar through reverb to dac

// set reverb mix
.1 => reverb.mix;

// modal bar parameters
7 => modal.preset;
.9 => modal.strikePosition;

// spork detune() as a child shred
// note: must do this before entering into infinite loop below!
spork ~ detune();                     // (2) Sporks a function called detune()

// infinite time loop
while( true )
{
    // first note! then wait
    1 => modal.strike;               // (3) Play notes (strike bar).
    250::ms => now;
    .7 => modal.strike; // another note! then wait
    250::ms => now;
    .9 => modal.strike; // one more note! then wait
    250::ms => now;
    repeat( 4 ) // now play four quick notes
    {
        // note! and wait
        .5 => modal.strike;
        62.5::ms => now;
    }
}

// function to vary tuning over time
fun void detune()                 // (4) detune function definition.
{
    while( true ) // infinite loop
    {
        // update frequency sinusoidally
        84 + Math.sin(now/second*.25*Math.PI) * 2 => Std.mtof => modal.freq;
        // advance time (controls update rate)
        5::ms => now;
    }
}
