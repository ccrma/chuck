class Foo
{
    int theX;

    // a constructor
    fun Foo( int x ) { x => theX; }
    
    // another constructor
    fun Foo()
    {
        // call a specific constructor
        Foo( 5 );
    }
}

// make a foo
Foo foo;

// check
if( foo.theX == 5 ) <<< "success" >>>;

