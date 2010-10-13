// assignment to class members

class Obj { 

	int x;
	int y;

	public void set ( int ax, int ay ) { 
		ax => x;
		ay => y;
	}

}

Obj fum;
fum.set ( 1 ,2 );


if( fum.x == 1 ) <<<"success">>>;
