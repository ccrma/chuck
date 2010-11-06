// 32.ck : calling functions by chucking

// declare X
class X
{
    // member data
    float f;

    // function
    fun float foo( float a, float b )
    { return a + b; }
}

// instantiate a X
X x;

// calling X.foo by chucking
if( (( 1, 2 ) => x.foo) == 3 )
    <<<"success">>>;
