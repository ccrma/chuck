//-------------------------------------------------------
// name: foreach-2.ck
// desc: iterating over an array of values
//
// requires: chuck-1.5.0.8 or higher
//-------------------------------------------------------

// iterating over an array literal
for( int x : [ 1, 2, 3 ] )
{ 
    // do something with it
    <<< x >>>; 
}
