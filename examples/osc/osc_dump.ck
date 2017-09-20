// osc_dump.ck
// listen to all OSC messages on specified port

OscIn oin;
if(me.args()) me.arg(0) => Std.atoi => oin.port;
else 9999 => oin.port;
oin.listenAll();

OscMsg msg;

while(true)
{
    oin => now;
    
    while(oin.recv(msg))
    {
        chout <= msg.address <= " ";
        for(int n; n < msg.numArgs(); n++)
        {
            if(msg.typetag.charAt(n) == 'i')
                chout <= msg.getInt(n) <= " ";
            else if(msg.typetag.charAt(n) == 'f')
                chout <= msg.getFloat(n) <= " ";
            else if(msg.typetag.charAt(n) == 's')
                chout <= msg.getString(n) <= " ";
        }
        
        chout <= IO.nl();
    }
}

