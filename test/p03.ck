// fail-correct class with function definition outside of class decl.

class Obj { 
	int x;
	int y;
	public void foo( int b);
} 

void Obj::foo( int b ) { 
	b => x;
}

<<<"success">>>;