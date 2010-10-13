// class with forward referencing functions

class Sequence { 
	int note;
	dur hold;
	Sequence @ next;

	fun void addNote( int n, dur h) { 
		if ( next == null ) { 
			Sequence nx @=> next;
			nx.setNote( n , h );
		}	
		else next.addNote( n, h );
	}

	fun void setNote( int n, dur h ) {
		n => note; 
		h => hold;
	}

	fun void play() { 
		// blah play the note
		hold => now;
		if ( next != null ) next.play();
	}
	fun void playBackwards() { 
		if ( next != null ) next.play();
		//start the note;
		hold=>now;
	}

//	fun Sequence getLast
}

1.0::second => beat;
0.25::beat => quarter;

Sequence root;

root.addNote ( 60, quarter );
root.addNote ( 0, 2::quarter );
root.addNote ( 64, quarter );

root.play();

<<<"success">>>;