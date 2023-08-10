//---------------------------------------------------------------------
// name: event-extend2.ck
// desc: working with shreds and custom Event to control audio
//       the "main" shred sporks four child shreds, each of which 
//       sets an concurrent voice (using FrencHrn). the main shred
//       then signals the child shreds one by one, passing along
//       pitch and velocity info using the custom event we defined.
//---------------------------------------------------------------------

// define a custom event class
class TheEvent extends Event
{
    int pitch;
    float velocity;
}

// instantiate the event
TheEvent e;

// shared reverb
NRev reverb => dac;
// set mix
.15 => reverb.mix;

// handler
fun void hi( TheEvent e, int id )
{
    // voice
    FrencHrn f => reverb;
    
    while( true )
    {
        // wait on event
        e => now;
        // print the data contained in the event
        <<< "shred", id, ":", e.pitch, e.velocity >>>;
        
        // set the pitch
        e.pitch => Std.mtof => f.freq;
        // play it
        e.velocity => f.noteOn;
        
        // wait a bit
        150::ms => now;
        // note off
        f.noteOff( 0 );
    }
}

// spork
spork ~ hi( e, 1 );
spork ~ hi( e, 2 );
spork ~ hi( e, 3 );
spork ~ hi( e, 4 );

// yield to let sporked shreds to have a chance to run and wait on the event
me.yield();

// infinite time loop
while( true )
{
    // set pitch
    Math.random2( 48, 84 ) => e.pitch;
    // set velocity
    Math.random2f( .5,1 ) => e.velocity;
    // signal one waiting shred
    e.signal();
    // advance time
    600::ms => now;
}
