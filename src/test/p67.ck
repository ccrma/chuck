// linked list stack;

class ListItem { 
	int data;
	null @=> ListItem @ next;
	fun void push ( ListItem it ) { 
		if ( next != null ) next.push(it);
		else it @=> next;
	} 
	fun int isLast() { 
		return ( next == null ) ? 1 : 0 ;
	}

	fun ListItem pop () { 
		if ( next.isLast() != 0) {
			next @=> ListItem @ n;
			null @=> next; 
			return n;
		}
		else return next.pop();
	}

}

class Stack { 
	int num;
	ListItem @ root;
	fun void push( ListItem it) { 
		if ( root == null ) it @=> root;
		else root.push(it);
	}
	fun ListItem pop() { 
		if ( root == null ) return null;
		else if ( root.isLast() != 0 ) { 
			root @=> ListItem @ ret ;
			null @=> root;
			return ret;
		}
		else { 
			return root.pop();
		}
	}
}

Stack f;

f.push( ListItem n );
f.push( ListItem p );

<<<"success">>>;