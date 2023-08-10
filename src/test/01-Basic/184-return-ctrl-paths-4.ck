// pass case: slightly more complex
// thanks @phlub on ChucK Community Discord Server

class Foo
{
    int chord[];
    int trailingNotes[];
    
    fun static Foo make( int c[], int t[])
    {
        Foo a;
        c @=> a.chord;
        t @=> a.trailingNotes;
        return a;
    }
}

// success condition
if( Foo.make([1,2],[3,4]).chord[1] == 2 )
    <<< "success" >>>;
