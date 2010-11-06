// triple-recursion

class Macro { 
	class Mini {
		float moog;
		class Micro { 
			int man;
		}		
		Micro moon; 	
		int foo;
	}
	Mini mia;
}

Macro mama;
<Macro.Mini> min;
<Macro.Mini.Micro> mic;
//assignments
5 => mic.man;
mic @=> min.moon;
min @=> mama.mia;

if ( mama.mia.moon.man == 5 ) <<<"success">>>;
