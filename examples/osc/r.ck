//----------------------------------------------------------------------------
// name: r.ck ('r' is for "receiver")
// desc: OpenSoundControl (OSC) receiver example
// note: launch with s.ck (that's the sender)
//----------------------------------------------------------------------------

// the patch
SinOsc s => JCRev r => dac;
.5 => s.gain;
.1 => r.mix;

// create our OSC receiver
OscIn oin;
// create our OSC message
OscMsg msg;
// use port 6449 (or whatever)
6449 => oin.port;
// create an address in the receiver, expect an int and a float
oin.addAddress( "/foo/notes, i f" );

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
        <<< "got (via OSC):", i, f >>>;
    }
}
