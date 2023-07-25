//-------------------------------------------------------
// name: foreach-1.ck
// desc: using for-each construct to loop over array
//
// requires: chuck-1.5.0.8 or higher
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
