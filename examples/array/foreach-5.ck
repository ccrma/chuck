//-------------------------------------------------------
// name: foreach-5.ck
// desc: foreach with Objects
//
// requires: chuck-1.5.0.8 or higher
//-------------------------------------------------------

// create empty array
SinOsc array[5];

// number
110 => float f;

// for each element 'x' in array
for( SinOsc x : array )
{
    // set the frequency
    f => x.freq;
    // double it
    2 *=> f;
}


// a second loop for testing's sake
// (also using Osc, a parent of SinOsc to iterate)
for( Osc x : array )
{
    // print
    <<< x.freq() >>>;
}
