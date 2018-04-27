// 11.ck : function with return type

// function
fun float f()
{
    return 1.0;
}

// call
f() => global float x;

// test
if( x == 1.0 ) <<<"success">>>;
