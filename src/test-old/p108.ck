// triple-recursion

class Macro { 
	class Mini {
		float fum;
		class Micro { 
			int foo;
		}		
		Micro men; 	
		int foo;
	}
	Mini mia;
}

Macro mama;
<Macro.Mini> min;
<Macro.Mini.Micro> mic;
//assignments
5 => mic.foo;
mic @=> min.men;
min @=> mama.mia;

<<<"success">>>;
