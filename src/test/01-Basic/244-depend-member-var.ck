// test declaring class members in a file below access

class Foo
{
    // member variable, declare above access in ctors
    int m_a;

    // a constructor
    fun @construct( int n ) { n => m_a => m_b; }
    // another constructor
    fun @construct( int a, int b ) { a => m_a; b => m_b; }

    // member variable, declare below access in ctrs
    int m_b;
}

// instantiate with 
Foo foo( 1, 2 );

if( foo.m_a + foo.m_b == 3 ) <<< "success" >>>;
