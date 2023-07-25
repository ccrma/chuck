//-------------------------------------------------------
// name: foreach-3.ck
// desc: nested for-each loops
//
// requires: chuck-1.5.0.8 or higher
//-------------------------------------------------------

// two arrays
[1, 2, 3] @=> int foo[];
[1, 2, 3] @=> int bar[];

// for each element in foo
for( int f : foo ) 
{
    // for each element in bar
    for( int b : bar )
    {
        // print the product
        <<< f*b >>>;
    }
}
