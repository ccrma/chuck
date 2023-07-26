//-------------------------------------------------------
// name: foreach-auto1.ck
// desc: using for-each construct with auto type
//
// requires: chuck-1.5.0.8 or higher
//-------------------------------------------------------

// create array
[1,2,3] @=> int array[];

// for each element 'x' in array
for( auto x : array )
{
    // print the element
    <<< x >>>;
}
