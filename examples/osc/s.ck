// launch with r.ck

// name
"localhost" => string hostname;
6449 => int port;

// check command line
if( me.args() ) me.arg(0) => hostname;
if( me.args() > 1 ) me.arg(1) => Std.atoi => port;

// send object
OscSend xmit;

// aim the transmitter
xmit.setHost( hostname, port );

// infinite time loop
while( true )
{
    // start the message...
    // the type string 'i f' expects a int, float
    xmit.startMsg( "/foo/notes", "i f" );

    // a message is kicked as soon as it is complete 
    // - type string is satisfied and bundles are closed
    Math.random2( 30, 80 ) => xmit.addInt;
    Math.random2f( .1, .5 ) => xmit.addFloat;

    // advance time
    0.2::second => now;
}
