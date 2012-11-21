// 19.ck : function that takes array as argument

// function foo
fun void foo( float a[] )
{
    // use 'a' associative
    10.0 => a["foo"];
}

// declare
float x[10];

// call foo
foo( x );

// test
if( 10.0 == x["foo"] ) <<<"success">>>;
