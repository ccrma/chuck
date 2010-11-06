// the patch 
// launch with opensound_demo_send.ck
sndbuf buf => dac;

//create our OSC receiver
OSC_Recv orec;
6449 => orec.port;  //use port 6449
orec.listen(); //start listening ( launches thread ) 

function void receiver() { 
	//create an address in the receiver 
	//and store it in a new variable.\
	orec.event("/bang/me,if") @=> OSC_Addr rate_addr; 
	while ( true ) { 

		rate_addr => now; //wait for Events to arrive.

		//grab the next message from the queue. 
		while ( rate_addr.nextMesg() != 0 ) { 
			rate_addr.getInt() => int hi;
			rate_addr.getFloat() => float span;
			<<<hi>>>;
			//fake your own death
			if ( std.rand2f(0.0, 1.0) < 0.07 ) { 
				<<<"stall">>>;
				0.5::second => now;
			}
		}
	}	
}

function void sender( float span ) { 
	OSC_Send xmit;
	xmit.setHost( "localhost", 6449 );
	0 => int i;
	while ( true ) { 
		xmit.startMesg( "/bang/me", ",if" );
		xmit.addInt(i);
		xmit.addFloat(span);
		i++;
		span::ms => now;
	}
}



// start our listening shred
spork ~ receiver();
spork ~ sender(10);
// time loop
while( true )
{
	1::second => now;
}
