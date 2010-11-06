
class Player { 
	UGen @ base;
	fun void connect( UGen target ) {
		base => target;
	}
	fun void noteOn ( float note, float vel ) {}
	fun void noteOff ( float vel ) {}
}

class Note { 

	float note;
	float vel;
	dur length;

	fun void set ( float nt, float vl, dur ln ) { 
		nt => note;
		vl => vel;
		ln => length;
	}

	fun void playOn ( Player p) { 
		if ( note > 0 ) { 
			p.noteOn( note , vel );
		}
	}

	fun void playOnAlt( Player p, float noff, float vmul ) { 
		p.noteOn( note+noff, vel*vmul );
	}

}


class MandPlayer extends Player { 
	Mandolin m @=> base; 
	fun void noteOn ( float note, float vel ) { 
		std.mtof ( note ) => m.freq;
		vel => m.pluck;
	}
}

class FlutePlayer extends Player { 
	PercFlut f @=> base; 
	fun void noteOn ( float note, float vel ) { 
		std.mtof ( note ) => f.freq;
		vel => f.noteOn;
	}
}



12 => int seqn;
Note box[100];
int order[12];


<<<"beep!">>>;
