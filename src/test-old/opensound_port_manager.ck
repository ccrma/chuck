// ( test port management )

// create our OSC receiver
OSC_Recv orec;
orec.address( "/test/me", "i" ) @=> OSC_Addr target;
// use port 6449

spork ~oscsrcs( 5::second );
spork ~oscsinks( 5::second, target );

// start listening (launch thread)
orec.listen( 6000 );

1::second => now;

orec.listen( 7000 );

1::second => now;

orec.listen( 8000 );

1::second => now;

orec.stop();

1::second => now;

fun void oscsrcs ( dur d ) { 
	now + d => time stop;
	OSC_Send x6;
	OSC_Send x7;
	OSC_Send x8;
	x6.setHost("localhost", 6000 );
	x7.setHost("localhost", 7000 );
	x8.setHost("localhost", 8000 );
	while ( now < stop ) { 
		x6.startMesg("/test/me", "i");
		x7.startMesg("/test/me", "i");
		x8.startMesg("/test/me", "i");
		6000 => x6.addInt;	
		7000 => x7.addInt;	
		8000 => x8.addInt;	
		100::ms => now;
	}

}

fun void oscsinks( dur d, OSC_Addr @ addr ) { 
	now + d => time stop;
	while ( now < stop ) {  
		addr => now;
		while ( addr.nextMesg() != 0 ) { 
			<<<"received...", addr.getInt()>>>;
		}
	}
}
