// sound chain
SinOsc s => dac; 

// initialize variable i
20 => int i; 

// loop 
while( i < 400 ) 
{
    i => s.freq; // set freq
    <<< i >>>; // print
    .01::second => now; // move time
    i++; // updater 
}