// error case: missing return
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
        // return a;
    }
}

<<< Foo.make( [1,2], [3,4] ) >>>;
