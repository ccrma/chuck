// pass case: slightly more complex
// related: see 184-return-ctrl-paths-4.ck

class Foo
{
    int chord[];
    int trailingNotes[];
    
    fun static Foo make( int c[], int t[])
    {
        Foo a;
        c @=> a.chord;
        
        // nested function definition
        fun void bar() { }
        
        t @=> a.trailingNotes;
        return a;
    }
}

// success condition
if( Foo.make([1,2],[3,4]).chord[1] == 2 )
    <<< "success" >>>;
