// (launch with OSC_send.ck)

// the patch
SndBuf buf => dac;
// load the file
me.dir(-1) + "data/snare.wav" => buf.read;
// don't play yet
0 => buf.play; 

// create our OSC receiver
OscIn oin;
// create our OSC message
OscMsg msg;
// use port 6449
6449 => oin.port;
// create an address in the receiver
oin.addAddress( "/sndbuf/buf/rate, f" );

// infinite event loop
while ( true )
{
    // wait for event to arrive
    oin => now;

    // grab the next message from the queue. 
    while ( oin.recv(msg) != 0 )
    { 
        // getFloat fetches the expected float (as indicated by "f")
        msg.getFloat(0) => buf.play;
        // print
        <<< "got (via OSC):", buf.play() >>>;
        // set play pointer to beginning
        0 => buf.pos;
    }
}
