// linked list Lifo;

class ListItem { 
	int data;
	ListItem @ next;
	fun void push ( ListItem it ) { 
		if ( next != null ) next.push(it);
		else it @=> next;
	} 
	fun int isLast() { 
		return ( next == null ) ? 1 : 0 ;
	}
	fun ListItem pop () { 
		if ( next.isLast() != 0 ) {
			next @=> ListItem @ n;
			null @=> next; 
			return n;
		}
		else return next.pop();
	}

}

class Lifo { 
	int num;
	ListItem @ root;
	fun void push( ListItem it) { 
		root @=> it.next;
		it @=> root;
	}
	fun ListItem pop() { 
		if ( root == null ) return null;
		root @=> ListItem @ ret;
		root.next @=> root;
		return ret;
	}
}

Lifo f;

f.push( ListItem n );
f.push( ListItem p );
f.pop() @=> ListItem @ l;

<<<"success">>>;