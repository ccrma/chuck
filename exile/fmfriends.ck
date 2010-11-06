// dut dut dut dut dut dut doot doot doot doot doot doot doot

FMVoices left=> gain g => dac ;
PercFlut  right=> gain g2 => dac ;


0.7 => left.gain;
0.7 => right.gain;

0.3 => g.gain;
0.5 => g2.gain;
0.2 => float mag;
333::ms => dur onebeat;
onebeat * 0.75 => dur notehold;
onebeat * 0.25 => dur noterest;

0.0 => float transp;

fun void fingers( float ln, float rn ) { 
	std.rand2f(0.0, 0.90 ) => left.spectralTilt;
	std.mtof(transp + ln) => left.freq;
	mag => left.noteOn;
	std.mtof(transp + rn) => right.freq;
	mag => right.noteOn;
	notehold => now;
	0.5 => left.noteOff;
	0.5 => right.noteOff;
	
}

0 => int i;
// our main loop
while( true )
{

	//6x f, g    65 67
	for ( 0=>i ; i < 6 ; i++ ) { 
		fingers(65.0, 67.0);
		noterest => now;
	}

	//6x e, g    64 67
	for ( 0=>i ; i < 6 ; i++ ) { 
		fingers(64.0, 67.0 );
		noterest => now;
	}

	//6x d, b    62 71
	for ( 0=>i ; i < 4 ; i++ ) { 
		fingers(62.0, 71.0 );
		noterest => now;
	}

        fingers(62.0, 69.0 );
        noterest => now;
        fingers(62.0, 71.0 );
        noterest => now;

	//4x  c, c    60 72
	for ( 0=>i  ; i < 4 ; i++ ) { 
		fingers(60.0, 72.0 );
		noterest => now;
	}

	if ( std.rand2(0, 4) > 2 ) { 		
		12 * std.rand2( -2, 1 ) => int freq;
		onebeat * 2.0 => now;
		1::second / (float) std.rand2(2,4)  => onebeat;
		onebeat * 0.75 => notehold;
		onebeat * 0.25 => noterest;
		(float)freq => transp;
	}
	else { 
		// d,b 
		fingers(62.0, 71.0 );
		noterest => now;

		// e,a
		fingers(64.0, 69.0 );
		noterest => now;
	}

}
