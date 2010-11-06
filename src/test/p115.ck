//virtual table

class Top { 
	function int topValue() { 
		return 1;
	}
}

class Bottom extends Top { 
	function int bottomValue() {
		return 2;
	}
}

Top t;
Bottom b;
b @=> Top @ b2t;


<<<"Bottom Class">>>;
<<<b.topValue()>>>;
<<<b.bottomValue()>>>;

<<<"Top Class">>>;
<<<t.topValue()>>>;

<<<"Bottom Cast to Top">>>;
<<<b2t.topValue()>>>;

if( b.topValue() == 1 && b.bottomValue() == 2 && t.topValue() == 1 && b2t.topValue() == 1 )
    <<<"success">>>;
