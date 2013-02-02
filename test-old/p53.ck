// implicit casts between int and float
// - test isn't proper just yet


fun void takesInts ( int argI ) { 
	argI => int localI;
}

fun void takesFloats ( float argF ) { 
	argF => float localF;
}


takesFloats( 4 );
takesInts(5.0 $ int);

1 => int a;
2 => float x;

takesFloats(a);
takesInts(x $ int);

a => float y;
y $ int => int b;

(5 * x) $ int => int c;
(x * 5) $ int => int d;
(3.0 * x) $ int => int e;
(x * 3.0) $ int => int f;

3 * a => float m;
a * 3 => float n;
5.0 * a => float o;
a * 5.0 => float p;

<<<"success">>>; 
