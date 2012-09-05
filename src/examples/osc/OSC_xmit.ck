// launch with OSC_recv.ck

// hostname and port
"localhost" => string hostname;
6449 => int port;

// get command line
if( me.args() ) me.arg(0) => hostname;
if( me.args() > 1 ) me.arg(1) => Std.atoi => port;

// the sender
OscSend xmit;

// aim the transmitter
xmit.setHost( hostname, port );

// stuff
0.0 => float running;
0.0 => float modrunning;
0 => int ct;
0.25 => float res;

[55, 57, 55, 58, 55, 59, 53] @=> int notes[];
7 => int nnote;
0 => int idx;

// infinite time loop
while( true )
{
    float f;
    if ( ct % 4 != 3 ) { 
        Math.random2f( 0.90, 1.10 ) => f;
    } else { running - modrunning => float diff;
        ( 1.0 + diff ) / res => f; 
    }

    xmit.startMsg ( "/sndbuf/buf/play", "f" );
    f => xmit.addFloat;
    res * f::second => now;
    res * f +=> modrunning; // actual time
    res +=> running; // track time
    1 +=> ct;
}
