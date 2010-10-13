class Panner extends gain { 
	
	0.5 => float mixval; 
	0   => int powermode;

	//this => 
	gain l_out;
	//this => 
	gain r_out;

	3 => l_out.op;
	3 => r_out.op;

	//we could just manage this via gain
	//but this would be smoother...

	Envelope l_env => l_out;
	Envelope r_env => r_out;
	
	0.02 => l_env.time;
	0.02 => r_env.time;

	function void setmeup()   { 	
		this => l_out;
		this => r_out;
	} 
	
	function gain left()  { return l_out; }
	function gain right() { return r_out; }

	function void mix( float m ) { 

		math.min( 1.0, math.max (0.0 , m ) ) => mixval;

		if ( powermode == 1 ) {
			math.sqrt ( 1.0 - mixval ) => l_env.target; 
			math.sqrt ( mixval ) => r_env.target; 
		}
		else { 
			1.0 - mixval => l_env.target;
			mixval => r_env.target;
		}
	}

}

Panner p; 
p.setmeup();
p.left() => dac;
Mandolin mand => dac;


440.0 => mand.freq;

while ( true ) { 

	now / ( 1.0::second ) => float fsec;
	<<<fsec>>>;
	0.5 + 0.5 * math.sin( fsec ) => p.mix;
	0.9 => mand.pluck;
	0.125::second => now;
}
