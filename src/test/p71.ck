// function sum modifies an argument passed to the function

class Point { 
	float p[3];
	fun void set ( float x, float y, float z ) { 
		x => p[0];
		y => p[1];
		z => p[2];
	}
	fun void setA ( float np[] ) { 
		for ( 0 => int i; i < 3; i +=> i ) np[i] => p[i];
	}
	fun void sum ( Point b , Point ret ) { 
		for ( 0 => int i; i < 3 ; 1 +=> i )  { 
			p[i] + b.p[i] => ret.p[i];
		}
	}
}

Point p1;
Point p2;
Point result;
p1.set( 0, 1, 2 );
p2.set( 5, 6, 7 );
p1.sum(p2, result);

if ( result.p[2] == 9 ) <<<"success">>>;

