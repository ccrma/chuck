// member set & get

class Obj { 
	int x;
	int y;
	public void set( int a, int b) { 
		a => x;
		b => y;
	}
	public int xval() { 
		return x;
	}
} 

Obj o;
o.set( 1 , 2 );
o.xval() => int out;

if ( out == 1  ) <<<"success">>>;


