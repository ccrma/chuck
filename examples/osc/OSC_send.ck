// launch with OSC_recv.ck

// host name and port
"localhost" => string hostname;
6449 => int port;

// get command line
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
    xmit.start( "/sndbuf/buf/rate" );
    
    // add float arg
    Math.random2f( .5, 2.0 ) => float temp => xmit.add;
    
    // send
    xmit.send();
    <<< "sent (via OSC):", temp >>>;

    // advance time
    0.2::second => now;
}
