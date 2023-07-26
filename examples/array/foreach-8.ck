//-------------------------------------------------------
// name: foreach-6.ck
// desc: foreach with unit generators
//
// requires: chuck-1.5.0.8 or higher
//-------------------------------------------------------

// create empty array
TriOsc array[4];

// for each, connect and set params
for( Osc x : array )
{ 
    // connect
    x => dac;
    // set gain
    .25 => x.gain;
    // set randomize freq
    Math.random2f(200,800) => x.freq;
}

// advance time
2::second => now;
