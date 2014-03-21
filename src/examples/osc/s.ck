// launch with r.ck

// name
"localhost" => string hostname;
6449 => int port;

// check command line
if( me.args() ) me.arg(0) => hostname;
if( me.args() > 1 ) me.arg(1) => Std.atoi => port;

// send object
OscOut xmit;

// aim the transmitter
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
