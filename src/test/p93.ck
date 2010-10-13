// class with function definition outside of class decl. ( fail-correct )

class Obj { 
	int x;
	int y;
	public void foo( int b);
} 

void Obj::foo( int b ) { 
	b => x;
}

<<<"success">>>;