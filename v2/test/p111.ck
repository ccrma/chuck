// access inner class data from newly returned object.

class Obj { 
	int val;
	class Sub { 
		int segment;
	}
	Sub inner;
	function Sub getInner() { 
	  return inner;
	}
}

fun Obj gimme() { 
	10 => (Obj r).val;
	20 => (<Obj.Sub> nin).segment;
	nin @=> r.inner;
	return r;
}

if ( gimme().getInner().segment == 20 ) <<<"success">>>;

