// recursive pile with fake subclass ( fail-correct )

class Macro { 
	class Mini {
		float fum;
		class Micro { 
			int foo;
		}		
		Micro m; 	
		int foo;
	}
	Mini moo;
}


<<<"success">>>;
