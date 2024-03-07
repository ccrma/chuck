// test OSC wildcards pattern matching
// added 1.5.2.2

OscIn oin;
OscMsg msg;

// the port
12003 => int OSC_PORT;
OSC_PORT => oin.port;

// address to listen for, with wildcard
oin.addAddress( "/test/*, is" );

// spork sender
spork ~ send();

// wait on OSC
oin => now;

// get the message
oin.recv(msg);

// test
if( msg.getInt(0) == 42 && msg.getString(1) == "hello!" )
{ <<< "success" >>>; } else { <<< "failure" >>>; }

// send function
fun void send()
{
    1::samp => now;
    
    while(true)
    {
        OscOut xmit;
        xmit.dest( "localhost", OSC_PORT );
        // OSC address
        xmit.start( "/test/foo" );
        // add data
        xmit.add(42);
        xmit.add("hello!");
        // send
        xmit.send();
        // wait
        1::second => now;
    }
}
