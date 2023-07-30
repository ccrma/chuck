// a 'repeat' loop repeats for a fixed number of repetitions

// our counter
0 => int x;

// repeat a fixed number of times
repeat( 4 )
{
    x++;
}

// repeat x number of times (or what the value of x is when we first
// reach this loop...e.g., should be 4 in this instance)
repeat( x )
{
    // the repeat value 'x' is evaluated only once at the beginning
    // of the loop; even if 'x' is subsequently changed, it will not
    // alter the number of repetitions of the loop
    x++;
}

// verify where the counter ended up
if( x == 8 ) <<< "success" >>>;
