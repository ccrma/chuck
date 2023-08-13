// using Std.range() to generate arrays containing a range of values
// (requires chuck-1.5.1.1 or higher)

// *** VARIANT #1 Std.range(stop) ***
// [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, ]
print( Std.range(10) );
// [ 0, -1, -2, -3, -4, ]
print( Std.range(-5) );

// *** VARIANT #2 Std.range(start,stop) ***
// [ 5, 6, 7, ]
print( Std.range(5,8) );
// [ 3, 2, 1, 0, -1, -2, -3, ]
print( Std.range(3,-4) ); // descending

// *** VARIANT #2 Std.range(start,stop,step) ***
// [ -1, 0, 1, 2, 3, 4, ]
print( Std.range(-1,5,1) );
// [ -2, -3, -4, -5, -6, -7, -8, -9, ]
print( Std.range(-2,-10,-1) ); // descending
// [ 0, 4, 8, ]
print( Std.range(0,10,4) ); // step of 4
// [ 5, 1, -3, -7, -11, -15, -19, ]
print( Std.range(5,-20,-4) ); // step of -4
// [ 5, 1, -3, -7, -11, -15, -19, ]
print( Std.range(5,-20,4) ); // same as 4

// validate zero length
// [ ]
print( Std.range(1,1,2) );
// [ ]
print( Std.range(1,1,-1) );
// [ ]
print( Std.range(-1,-1,-1) );
// step == 0, results in [ ]
print( Std.range(1,1,0) );

// formatted array print
fun void print( int array[] )
{
    // print open
    cherr <= "[ ";
    // print each element
    for( int x : array ) cherr <= x <= ", ";
    // print close
    cherr <= "]";
    // new line
    cherr <= IO.nl();
}
