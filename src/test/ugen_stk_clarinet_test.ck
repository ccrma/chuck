
0 => int i;
6 => int chorus; 
Clarinet horns[chorus];

gain g => JCRev j => Echo e => dac;

for ( 0 => int i  ; i < chorus ; 1 +=> i  ) { 
	Clarinet br @=> horns[i];
	440.0 => br.freq;
	40.0 => br.rate;
	br => g;
}

0.10 => g.gain;

1.0::second => e.max ;
0.750::second => e.delay;
0.1 => e.mix;
0.2 => j.mix;
0.97 => j.gain;




[0, 2, 4, 7, 9, 12, 14 ,16, 19 ,21] @=> int f[];

while( true )
{
	for (  0 => i ;  i< chorus ; 1 +=> i  ) { 
		std.mtof ( 65 +  f[std.rand2(0,9)] ) => horns[i].freq;
		std.rand2f( 0.8, 0.9 ) => horns[i].startBlowing;
	}
	0.125::second => now;	
	for ( 0 => i ; i < chorus ; 1 +=> i ) { 
		if ( std.rand2( 0 , 10 ) > 3 ) { 
			horns[i].stopBlowing(10.0);
		}
	}
	0.0625::second => now;
//	<<<"beep">>>;
}
