// 35.ck : overloading functions


fun void foo( int a, int b )
{ <<<"success">>>; }

fun void foo( int a )
{ foo( a, a ); }

// call
foo( 2 );
