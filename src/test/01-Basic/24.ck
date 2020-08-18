// 24.ck : class with a member field and a member function

class X
{
    // data
    int i;

    // function
    public void foo( int a )
    {
        // set i
        a => i;
    }
}

// instantiate a X
X x;

// call
x.foo( 2 );

// test
if( x.i == 2 ) <<<"success">>>;
