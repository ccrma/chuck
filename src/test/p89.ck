// this

class Piece { 
	int part;
	fun void set ( int part ) { 
		part => this.part;
	}
}

Piece p;
p.set( 5 );

if ( p.part == 5 ) <<<"success">>>;