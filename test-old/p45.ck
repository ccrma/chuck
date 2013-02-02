// function taking float arg, returning float

fun float squared( float arg )
{
    return arg*arg;
}

squared(1.6) => float x;

<<<x>>>;
if ( x != 2.56 ) { 
	<<<"this is due to double precision math:  1.6 x 1.6 != 2.56 ">>>;
}

if ( x - 2.56 < .000001 ) { 
	<<<"success">>>;
}
