//virtual table

class AClass { 
	function int value() { 
		return 1;
	}
	function static int staticvalue() { 
		return 2;
	}
}

AClass boing;

<<<"Test static value">>>;
<<<boing.value()>>>;
<<<boing.staticvalue()>>>;
<<<AClass.staticvalue()>>>;
