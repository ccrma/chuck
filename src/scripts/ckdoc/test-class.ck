public class Foo
{
    /** a is an integer; nothing uses this. */
    int a;
    /** b is a float; nothing uses this either. */
    float b;
    /** a number, type integer */
    1 => int num;
    
    /** constructor "default" */
    fun @construct() { 2 => num; }
    
    /** another constructor */
    fun @construct( int x ) { x => num; }
    
    /** yet another constructor */
    fun @construct( int x, int y ) { x*y => num; }
    
    /** alternate way of define a constructor */
    fun void Foo( int x, int y, int z ) { x*y*z => num; }

    /** this function does nothing. */
    fun void bar( int x )
    {
        // \o/ this function does nothing" \o/
        // => this function does nothing" <=
        //=> this function does nothing
        /*=> */
    }

    /** destructor */
    fun @destruct() { <<< "destructor:", this.num >>>; }
}

// get Type
[ Foo.typeOf() ] @=> Type types[];
// doc generator
CKDoc doc;
// add group
doc.addGroup( types, "my classes!", "my", "Will this work?" );
// generate
doc.outputToDir( ".", "My Class Library" );
