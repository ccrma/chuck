
OscIn oin;
OscMsg msg;

12002 => int OSC_PORT;

OSC_PORT => oin.port;
oin.addAddress( "/test, i f s" );

spork ~ send();

oin => now;

oin.recv(msg);

if(msg.getInt(0) == 42 && Math.fabs(msg.getFloat(1) - 3.141) < 0.001 && msg.getString(2) == "hello!")
    <<< "success" >>>;
else
    <<< "failure" >>>;

fun void send()
{
    1::samp => now;
    
    while(true)
    {
        OscOut xmit;
        xmit.dest( "localhost", OSC_PORT );   
        xmit.start( "/test" );
        xmit.add(42);
        xmit.add(3.141);
        xmit.add("hello!");
        xmit.send();
        
        1::second => now;
    }
}


