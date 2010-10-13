// function taking 2 float args, returning float ( fail-correct )

fun float multiply( float arg1, float arg2 )
{
    return arg1*arg2;
}

multiply(0.8, 1.6) => float x;

if ( x == 1.28 ) { 
	<<<"success">>>;
}
