// this

class Part { 
	int foo;
	Part @ child;
	fun Part end ( ) { 
		if ( child != null ) return child.end();
		return this;
	}
	fun void makeChild() { 
		Part ch @=> this.child;
		foo*2 => ch.foo;
	}
}

class Bunch { 
	Part @ tail;
	fun void setTail ( Part p ) { 
		p.end() @=> this.tail;
	}
}

Bunch b;
Part p;
2 => p.foo;
b.setTail ( p );

if ( b.tail.foo == 2 ) <<<"success">>>;
