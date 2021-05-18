// osc_dump.ck
// listen to all OSC messages on specified port

// OSC in
OscIn oin;
// see if port is supplied on command line
if( me.args() ) me.arg(0) => Std.atoi => oin.port;
// default port
else 9999 => oin.port;
// listen to everything coming
oin.listenAll();

// something to shuttle data
OscMsg msg;

// infinite time loop
while(true)
{
    // wait for OSC input
    oin => now;
    
    // drain the message queue
    while( oin.recv(msg) )
    {
        // print address
        chout <= msg.address <= " ";
        // loop over number of arguments
        for(int n; n < msg.numArgs(); n++)
        {
            // print by type
            if(msg.typetag.charAt(n) == 'i')
                chout <= msg.getInt(n) <= " ";
            else if(msg.typetag.charAt(n) == 'f')
                chout <= msg.getFloat(n) <= " ";
            else if(msg.typetag.charAt(n) == 's')
                chout <= msg.getString(n) <= " ";
        }
        // print newline
        chout <= IO.nl();
    }
}
