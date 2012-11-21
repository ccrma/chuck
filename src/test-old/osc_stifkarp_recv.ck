
OSC_Recv recv;

6448 => recv.port;
recv.listen();

StifKarp stif => Echo a => JCRev r =>  dac;
0.95 => r.gain;
0.1 => r.mix; 
1.1::second => a.max;
1.0::second => a.delay;
0.2 => a.mix;

a => Echo b => a;
1.0::second => b.max;
0.3::second => b.delay;
0.5 => b.mix;
0.7 => b.gain;


function void hitnote( int note, int vel ) { 
	std.mtof( note ) => stif.freq;
	vel / 128.0 => stif.pluck;
}

function void play_trigger() { 
	recv.event ( "/voice/Stif/noteOn,ii" ) @=> OSC_Addr oscev;
	while ( true  ) { 
		oscev => now;
		while ( oscev.nextMesg() != 0 ) { 
			hitnote( oscev.getInt() , oscev.getInt() ); 
		}
	}
}


function void stretch_listener() { 
	recv.event( "/voice/Stif/stretch,f" ) @=> OSC_Addr oscv;
	while ( true  ) { 
		oscv => now;
		while ( oscv.nextMesg() != 0 ) { 
			oscv.getFloat() => float stf;
			stf => stif.stretch ; 
		}
	}
}


hitnote ( 60, 80 );

spork ~ play_trigger();
spork ~ stretch_listener();

[ 0, 2, 4, 7, 9, 12, 14, 16, 19, 21 ] @=> int fnotes[];

while ( true ) { 
	std.rand2( 0,9 ) => int n;
	hitnote ( 64 + fnotes[n] , 90 );
	if ( std.rand2(0,4) > 3 ) { 
		1.5::second => now;
	}		
	else { 
		std.rand2(0,9) => int g;
		0.125::second => now;
		hitnote ( 64 + fnotes[g], 60 );
		0.125::second => now;
		hitnote ( 64 + fnotes[n], 70 );
		1.25::second => now;		
	}	
}

