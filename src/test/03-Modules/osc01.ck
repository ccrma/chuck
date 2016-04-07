
OscIn oin;
OscMsg msg;

12001 => oin.port;
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
    
    OscOut xmit;
    xmit.dest( "localhost", 6449 );   
    xmit.start( "/test" ).add("hello!").send();
}


