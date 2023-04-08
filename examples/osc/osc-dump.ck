//----------------------------------------------------------------------------
// name: osc-dump.ck
// desc: listen to and print all OSC messages on specified port
//       for example, launch this with s.ck
//       (make sure port is the same in both sender and receiver)
//----------------------------------------------------------------------------

// OSC in
OscIn oin;

// see if port is supplied on command line
if( me.args() ) me.arg(0) => Std.atoi => oin.port;
// default port
else 6449 => oin.port;

// print
cherr <= "listening for OSC messages over port: " <= oin.port()
      <= "..." <= IO.newline();
// listen to everything coming
oin.listenAll();

// something to shuttle data
OscMsg msg;

// infinite time loop
while( true )
{
    // wait for OSC input
    oin => now;
    
    // drain the message queue
    while( oin.recv(msg) )
    {
        // print address and typetag
        cherr <= msg.address <= " " <= msg.typetag <= " ";
        // loop over number of arguments by type
        for(int n; n < msg.numArgs(); n++)
        {
            // print by type
            if(msg.typetag.charAt(n) == 'i')
                cherr <= msg.getInt(n) <= " ";
            else if(msg.typetag.charAt(n) == 'f')
                cherr <= msg.getFloat(n) <= " ";
            else if(msg.typetag.charAt(n) == 's')
                cherr <= msg.getString(n) <= " ";
        }
        // print newline
        cherr <= IO.nl();
    }
}
