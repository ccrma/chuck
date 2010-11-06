// recursive function

fun int factorial( int x )
{ 
    if ( x <= 1 ) return 1; 
    else return x * factorial ( x-1 );
}

// call
factorial ( 5 ) => int answer;

// test
if ( answer == 120 ) <<<"success">>>;

