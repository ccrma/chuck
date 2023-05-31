//----------------------------------------------------------------------------
// name: s-sender.ck
// desc: OSC example: one (sender) to many (receivers);
// note: launch with one r.ck running per receiver machine
//
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: spring 2022
//----------------------------------------------------------------------------

// destination port number
6449 => int port;

// array of destination hostnames
string hostnames[0];

//----------------------------------------------------------------------------
// appending names of destinations
//----------------------------------------------------------------------------

// localhost == this machine
hostnames << "localhost";

// external hosts (add as many as needed)
// hostnames << "albacore.local";
// hostnames << "192.168.0.10";
// hostnames << "host.domain.edu";
// ...

// sender object
OscOut xmit[hostnames.size()];
// iterate over the OSC transmitters
for( int i; i < xmit.size(); i++ )
{
    // aim the transmitter at destination
    xmit[i].dest( hostnames[i], port );
}

// infinite time loop
while( true )
{
    // for each xmit
    for( int i; i < xmit.size(); i++ )
    {
        // start the message...
        xmit[i].start( "/foo/notes" );
        
        // add int argument
        Math.random2( 30, 80 ) => xmit[i].add;
        // add float argument
        Math.random2f( .1, .5 ) => xmit[i].add;
        
        // send it
        xmit[i].send();
    }

    // advance time
    250::ms => now;
}
