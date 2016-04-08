
OscIn oin;
OscMsg msg;

12001 => int OSC_PORT;

OSC_PORT => oin.port;
oin.addAddress( "/test" );

spork ~ send();

oin => now;

oin.recv(msg);

if(msg.getString(0) == "hello!")
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
        xmit.start( "/test" ).add("hello!").send();
        
        1::second => now;
    }
}


