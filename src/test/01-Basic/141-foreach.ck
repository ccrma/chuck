//-------------------------------------------------------
// name: foreach-4.ck
// desc: iterating over multi-dimensional arrays
//
// requires: chuck-1.5.0.8 or higher
//-------------------------------------------------------

// a multi dimensional array
int array[2][0];

// fill the two arrays
array[0] << 1 << 2;
array[1] << 3 << 4;

// loop over outer array
for( int x[] : array )
{
    // loop over inner array
    for( int y : x )
    {
        <<< y >>>;
    }
}
