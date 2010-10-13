class Panner extends UGen { 
	
	0.5 => float mixval; 
	0   => int powermode;

	this => Envelope l_env => UGen l;
	this => Envelope r_env => UGen r;
	
	0.02 => l_env.time;
	0.02 => r_env.time;
	0.5  => l_env.value;
	0.5  => r_env.value;

	function UGen left()  { return l; }
	function UGen right() { return r; }

	function void set( float m ) { 

		math.min( 1.0, math.max (0.0 , m ) ) => mixval;

		if ( powermode == 1 ) {
			math.sqrt ( 1.0 - mixval ) => l_env.value; 
			math.sqrt ( mixval )       => r_env.value; 
		}
		else { 
			1.0 - mixval => l_env.value;
			mixval       => r_env.value;
		}	
			
	}

	function void time ( float t ) { 
		t => l_env.time;
		t => r_env.time;
	}


	function void mix( float m ) { 

		math.min( 1.0, math.max (0.0 , m ) ) => mixval;

		if ( powermode == 1 ) {
			math.sqrt ( 1.0 - mixval ) => l_env.target; 
			math.sqrt ( mixval )       => r_env.target; 
		}
		else { 
			1.0 - mixval => l_env.target;
			mixval       => r_env.target;
		}

	}

}

Mandolin mand => Panner p;
p.l => dac;

440.0 => mand.freq;

while ( true ) { 

	now / ( 1.0::second ) => float fsec;
	0.5 + 0.5 * math.sin( fsec ) => p.mix;
	0.9 => mand.pluck;
	0.125::second => now;
}
