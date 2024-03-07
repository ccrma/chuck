//----------------------------------------------------------------------------
// name: s-multi.ck
// desc: OSC sender with multiple addresses
//----------------------------------------------------------------------------

// destination host name
"localhost" => string hostname;
// destination port number
6449 => int port;

// check command line
if( me.args() ) me.arg(0) => hostname;
if( me.args() > 1 ) me.arg(1) => Std.atoi => port;

// one send loop
spork ~ sendloop( "/foo/notes" );
// wait a bit
.2::second => now;
// another send loop with different address
spork ~ sendloop( "/foo/bar" );

// wait
while( true ) 1::second => now;

// send loop with OSC address
fun void sendloop( string oscAddress )
{
    // sender object
    OscOut xmit;
    // aim the transmitter at destination
    xmit.dest( hostname, port );
    // infinite time loop
    while( true )
    {
        // start the message...
        xmit.start( oscAddress );
        
        // add int argument
        Math.random2( 30, 80 ) => xmit.add;
        // add float argument
        Math.random2f( .1, .5 ) => xmit.add;
        
        // send it
        xmit.send();
        
        // advance time
        0.4::second => now;
    }
}
