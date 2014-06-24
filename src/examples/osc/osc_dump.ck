// osc_dump.ck
// listen to all OSC messages on specified port

OscIn oin;
if(me.args()) me.arg(0) => Std.atoi => oin.port;
else 6449 => oin.port;
oin.listenAll();

OscMsg msg;

while(true)
{
    oin => now;
    
    while(oin.recv(msg))
    {
        <<< msg.address, ",", msg.typetag >>>;
        for(int n; n < msg.numArgs(); n++)
        {
            if(msg.typetag.charAt(n) == 'i')
                <<< "arg", n, ":", msg.getInt(n) >>>;
            else if(msg.typetag.charAt(n) == 'f')
                <<< "arg", n, ":", msg.getFloat(n) >>>;
            else if(msg.typetag.charAt(n) == 's')
                <<< "arg", n, ":", msg.getString(n) >>>;
        }
    }
}

