// for loop ( factorial ) , -=>, *=>

// function
fun int factorial ( int arg )
{ 
    1 => int r;
    arg => int b;
    for ( b ; b > 1 ; 1 -=> b )
    { 
        b *=> r;
    }

    return r;
}

// call the function
factorial ( 4 ) => int x;

// test
if ( x == 24 ) <<<"success">>>;
