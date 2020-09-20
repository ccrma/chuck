// 28.ck : global variable

10 => global int z;

class X
{
    fun void foo()
    {
        20 => z;
    }
}

// instantiate a X
X x;

// call
x.foo();

// test
if( z == 20 ) <<<"success">>>;
