// member function assigns value to class members...
// should wait 1 second
class Obj { 
	int x;
	int y;
	public void set( int a, int b) { 
		a => x;
		b => y;
	}
} 

Obj o;
o.set( 1, 2 );
Obj p;
p.set( 3, 4 );

if ( o.x != p.x ) <<<"success">>>;
