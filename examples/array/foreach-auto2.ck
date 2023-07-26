//-------------------------------------------------------
// name: foreach-auto2.ck
// desc: iterating over multi-dimensional arrays
//.      using for-each and auto
//
// requires: chuck-1.5.0.8 or higher
//-------------------------------------------------------

// a multi dimensional array
int array[2][0];

// fill the two arrays
array[0] << 1 << 2;
array[1] << 3 << 4;

// loop over outer array
for( auto x[] : array )
{
    // loop over inner array
    for( int y : x )
    {
        <<< y >>>;
    }
}
