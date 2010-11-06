// member set & get
// test that different object don't share value

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
Obj p;
p.set( 3, 4 );

if ( o.xval() != p.xval()  ) <<<"success">>>;

