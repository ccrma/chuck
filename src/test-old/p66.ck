// linked list - addNote appends new element at the end of list

class Sequence { 
	int note;
	dur hold;
	Sequence @ next;
	fun void addNote( int n, dur h) { 
		if ( next == null ) { 
			Sequence nx @=> next;
			n => nx.note;
			h => nx.hold;
		}	
		else next.addNote( n, h );
	}
}

Sequence root;

0 => root.note;
0::second => root.hold;
root.addNote ( 60, 0.25::second );
root.addNote ( 0, 0.5::second );

<<<"success">>>;