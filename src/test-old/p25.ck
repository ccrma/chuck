// pass by value test

fun void fake_equals( float arg1, float arg2 )
{
	arg1 => arg2;
}

2 => float x;
3 => float y;

fake_equals( x, y );

if ( x != y ) { 
	<<<"success">>>;
}



