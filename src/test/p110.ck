// access member of object returned by function...

class Obj { 
	int val;
}

fun Obj gimme() { 
	10 => (Obj r).val;
	return r;
}

if ( gimme().val == 10 ) <<<"success">>>;

