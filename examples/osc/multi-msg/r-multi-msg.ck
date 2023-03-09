//----------------------------------------------------------------------------
// name: r-multimsg.ck
// desc: OSC example: receiver for multiple message types
// note: launch with s-multimsg.ck
//
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: spring 2022
//----------------------------------------------------------------------------

// the patch
BlitSaw s => JCRev r => dac;
.5 => s.gain;
.1 => r.mix;

// spork the handlers, one for each message type
spork ~ onNotes();
spork ~ onHarmonics();

// keep alive
while( true ) 1::second => now;

// handler for incoming OSC notes messages
fun void onNotes()
{
    // create our OSC receiver
    OscIn oin;
    // create our OSC message
    OscMsg msg;
    // use port 6449 (or whatever)
    6449 => oin.port;
    // create an address in the receiver, expect an int and a float
    oin.addAddress( "/foo/notes, if" );
    
    // infinite event loop
    while( true )
    {
        // wait for event to arrive
        oin => now;
        
        // grab the next message from the queue. 
        while( oin.recv(msg) )
        { 
            // expected datatypes (note: as indicated by "i f")
            int i;
            float f;
            
            // fetch the first data element as int
            msg.getInt(0) => i => Std.mtof => s.freq;
            // fetch the second data element as float
            msg.getFloat(1) => f => s.gain;
            
            // print
            <<< "notes (via OSC):", i, f >>>;
        }
    }
}

// handler for incoming OSC harmonics messages
fun void onHarmonics()
{
    // create our OSC receiver
    OscIn oin;
    // create our OSC message
    OscMsg msg;
    // use port 6449 (or whatever)
    6449 => oin.port;
    // create an address in the receiver, expect an int
    oin.addAddress( "/foo/harmonics, i" );
    
    // infinite event loop
    while( true )
    {
        // wait for event to arrive
        oin => now;
        
        // grab the next message from the queue. 
        while( oin.recv(msg) )
        { 
            // expected datatypes (note: as indicated by "i")
            int i;
            
            // fetch the first data element as int
            msg.getInt(0) => i => s.harmonics;
            
            // print
            <<< "harmonics (via OSC):", i >>>;
        }
    }
}
