// array pass by reference test - should change values?

fun void mirror( int arg[] )
{
	arg[0] => arg[1];
}

int foo[2];
2 => foo[0];
1 => foo[1];

mirror( foo );

if ( foo[0] == foo[1] ) { 
	<<<"success">>>;
}