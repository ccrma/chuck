//----------------------------------------------------------------------------
// name: s-multimsg.ck
// desc: OSC example: sender for multiple message types
// note: launch with r-multimsg.ck
//
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: spring 2022
//----------------------------------------------------------------------------

// destination host name
"localhost" => string hostname;
// destination port number
6449 => int port;

// check command line
if( me.args() ) me.arg(0) => hostname;
if( me.args() > 1 ) me.arg(1) => Std.atoi => port;

// sender object
OscOut xmit;

// aim the transmitter at destination
xmit.dest( hostname, port );

// spork sending loops, one for each message type
spork ~ sendNotes();
spork ~ sendHarmonics();

// keep allive
while( true ) 1::second => now;

fun void sendNotes()
{
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
}

fun void sendHarmonics()
{
    // infinite time loop
    while( true )
    {
        // start the message...
        xmit.start( "/foo/harmonics" );
        
        // add int argument
        Math.random2( 1, 8 ) => xmit.add;
        
        // send it
        xmit.send();
        
        // advance time
        2.4::second => now;
    }
}