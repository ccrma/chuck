//-------------------------------------------------------
// name: ctrl_foreach.ck
// desc: for-each construct to loop over arrays
//
// requires: chuck-1.5.0.8 or higher
//
// SEE ALSO: examples/array/foreach-*.ck for more
//-------------------------------------------------------

// create empty array
float array[0];

// append
array << 1 << 2.5 << 3;

// for each element 'x' in array
for( float x : array )
{
    // print the element
    <<< x >>>;
}

// iterating over an array literal
for( int x : [ 1, 2, 3 ] )
{ 
    // do something with it
    <<< x >>>; 
}
