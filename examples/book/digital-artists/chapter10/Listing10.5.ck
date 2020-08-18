// Listing 10.5 Using a custom Event subclass, with polymorphism
class TheEvent extends Event   // (1) Declares Event subclass with extra instance variables.
{
    int note;                  // (2) Integer note instance variable.
    float velocity;            // (3)Float velocity instance variable.
}

TheEvent evnt, evnt2;          // (4) Declares two new TheEvent objects.

// patch
NRev globalReverb => dac;      // (5) Declares a global reverb for use by all.
.1 => globalReverb.mix;

// instrument function to spork
fun void poly( StkInstrument instr, TheEvent e, string s )
{                              // (6) Declares an event-waiting, instrument-playing function.
    // connect to output
    instr => globalReverb;     // (7) Connects the instrument to your global reverb.
    // hang out waiting to receive an event to play
    while( true )              // (8) Infinite loop.
    {
        // wait
        e => now;              // (9) Waits for event to be signaled.
        // play
        e.note => Std.mtof => instr.freq;  // (10 When event comes, sets note frequency
        e.velocity => instr.noteOn;        // (11) ...and fires instrument with velocity.
    }
}

// spork a few polys, listening on "evnt"
spork ~ poly( new StifKarp, evnt, "StifKarp" );  // (12) Sporks an evnt-waiting function with a StifKarp instrument...                             
spork ~ poly( new Mandolin, evnt, "Mandolin" );  // (13) and another with a Mandolin instrument...
spork ~ poly( new Wurley, evnt, "Wurley" );      // (14) ...and yet another with an FM Wurley instrument.

// spork one poly listening on "evnt2"
spork ~ poly( new Rhodey, evnt2, "Rhodey" );     // (15) Sporks an evnt2-waiting function with a Rhodey instrument.

[60,62,64,67,69,72,74,76,79] @=> int notes[];    // (16) Notes scale for your gamelan (a scale is called a laya).

// play forever
while( true )
{                                   // (17) Infinite loop.
    // fire the next signal, on a dice roll
    Math.random2(1,6) => int dice;  // (18) Dice roll.
    if (dice != 1)
    {                               // (19) Five times out of 6, pick a note from the laya.
        // pick a random note from our array
        notes[Math.random2(0,notes.cap()-1)] => evnt.note;
        Math.random2f( .2, .9 ) => evnt.velocity;    // (20) Random velocity.

        // send the signal to only one instrument
        evnt.signal();              // (21) Signal event (one of StifKarp, Mandolin, Wurley)...

        // and advance time
        0.25 :: second => now;
    }
    else
    {              // (22) ...otherwise, pick a lower laya note and signal evnt2 (Rhodey)...

        // play a lower notes on evnt2, and all of the evnt instruments
        notes[Math.random2(0,notes.cap()-1)] - 24 => evnt2.note;
        notes[0] - 12 => evnt.note;
        1.0 => evnt2.velocity;

        // on all instrument shreds
        evnt.broadcast();           // (23) ...and broadcast to all evnt listener instruments.
        evnt2.signal();

        // and wait longer
        second => now;
    }
}

