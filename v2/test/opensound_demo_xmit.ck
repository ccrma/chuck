// patch to transmit OSC
// launch with opensound_demo_recv.ck

OSC_Send xmit;

//aim the transmitter at our local port 6449
xmit.setHost ( "localhost", 6449 );

// time loop
0.0 => float running;
0.0 => float modrunning;
0 => int ct;
0.25 => float res;

[55, 57, 55, 58, 55, 59, 53] => int notes[];
7 => int nnote;
0 => int idx;

while( true )
{
	float f;
	if ( ct % 4 != 3 ) { 
		std.rand2f( 0.90, 1.10 ) => f;
	}
	else { 
		running - modrunning => float diff;
		( 1.0 + diff ) / res => f; 
	}

	xmit.startMesg ( "/sndbuf/buf/play", ",f");
	f => xmit.addFloat;
	res * f::second => now;
	res * f +=> modrunning; //actual time
	res +=> running; //track time
	1 +=> ct;
}
