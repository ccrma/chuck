// sound chain
SinOsc s => dac; 

// loop 
for( 20 => int i; i < 400; i++ ) 
{
   <<< i >>>;  // print value of i
   i => s.freq; // set frequency 
   .01::second => now; // move time
}