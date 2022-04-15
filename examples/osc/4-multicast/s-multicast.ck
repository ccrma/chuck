//----------------------------------------------------------------------------
// name: s-multicast.ck
// desc: OSC example: multicasting to all clients on LAN
// note: launch with r.ck (one on each client on local area network)
//
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: spring 2022
//----------------------------------------------------------------------------

// multicast address sends to all machines on local network
"224.0.0.1" => string hostname;
// destination port number
6449 => int port;

// sender object
OscOut xmit;

// aim the transmitter at destination
xmit.dest( hostname, port );

// infinite time loop
while( true )
{
    // start the message...
    xmit.start( "/foo/notes" );
    
    // add int argument
    Math.random2( 30, 80 ) => xmit.add;
    // add float argument
    Math.random2f( .1, .5 ) => xmit.add;
    
    // send it
    xmit.send();

    // advance time
    0.2::second => now;
}
