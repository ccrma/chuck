public class Foo
{
    /** a is an integer; nothing uses this. */
    int a;
    /** b is a float; nothing uses this either. */
    float b;

    /** this function does nothing. */
    fun void bar( int x )
    {
        // \o/ this function does nothing" \o/
        // => this function does nothing" <=
        //=> this function does nothing
        /*=> */
    }
}

[ Foo.typeOf() ] @=> Type types[];

CKDoc doc;

doc.addGroup( types, "my classes!", "my", "Will this work?" );

// generate
doc.outputToDir( ".", "My Class Library" );
