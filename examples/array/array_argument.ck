// array pass by reference test

// function that takes array
fun void mirror( int arg[] )
{
    // assign 0th element to 1st element
    arg[0] => arg[1];
}

// declare array
int foo[2];
2 => foo[0];
1 => foo[1];

// call the function
mirror( foo );

// test
if ( foo[0] == foo[1] ) <<<"success">>>;
