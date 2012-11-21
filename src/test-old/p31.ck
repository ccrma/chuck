// member function assigns value to class members...

class Obj { 
	int x;
	int y;
	public void clear() { 
		0 => x;
		0 => y;
	}
} 

Obj o;
5 => o.x;
o.clear();

if ( o.x == 0 ) { 
	<<<"success">>>;
}


