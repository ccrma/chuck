// (launch with s.ck)

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
// create an address in the receiver
oin.addAddress( "/foo/notes, if" );

// infinite event loop
while( true )
{
    // wait for event to arrive
    oin => now;

    // grab the next message from the queue. 
    while( oin.recv(msg) )
    { 
        int i;
        float f;

        // getFloat fetches the expected float (as indicated by "i f")
        msg.getInt(0) => i => Std.mtof => s.freq;
        msg.getFloat(1) => f => s.gain;

        // print
        <<< "got (via OSC):", i, f >>>;
    }
}
