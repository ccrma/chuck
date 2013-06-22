// sound chain
SqrOsc s => dac;

// infinite loop 
while( true ) 
{
    Math.random2(20,500) => int i; // generate random int
    Math.random2f(.2, .9) => float v; // generate random float
    <<< i, v >>>; // print out variables  
    i => s.freq; // update frequency 
    v => s.gain; // update volume
    .5::second => now; // advance half a second
}



