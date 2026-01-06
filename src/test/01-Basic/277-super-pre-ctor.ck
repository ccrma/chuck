// name: super-1-simple.ck
// desc: simple usage of `super` keyword to access parent functions
// authors: Nick and Ge, Fall 2025

// a class
class X
{
    // a function
    fun void boo( int i ) { <<< "W", i >>>; }
}

// another class; a child of X
class Y extends X
{
    // calling superclass boo from class pre-constructor code
    super.boo(1);

    // overriding boo()
    fun void boo( int i )
    {
        <<< "Y", i >>>;
        // calling superclass boo from within a member function
        super.boo(2);
    }
}

// instantiate a Y
Y y;

// call
y.boo( 4 );
