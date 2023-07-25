//-------------------------------------------------------
// name: foreach-6.ck
// desc: testing NULL array
//
// requires: chuck-1.5.0.8 or higher
//-------------------------------------------------------

// create null array reference
float array[];

// null array will have no effect
for( float x : array )
{
    // print the element
    <<< x >>>;
}

// print
<<< "success" >>>;