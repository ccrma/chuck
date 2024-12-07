// test calling constructor using `this`

class Foo
{
    int theX;

    // a constructor
    fun Foo( int x ) { x => theX; }
    
    // another constructor
    fun Foo()
    {
        // call a specific constructor
        this( 5 );
    }
}

// make a foo
Foo foo;

// should print 5
if( foo.theX == 5 ) <<< "success" >>>;

