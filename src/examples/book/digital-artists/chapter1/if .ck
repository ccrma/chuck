// sound chain
SinOsc s => dac;
 
// setting frequency
222.45 => s.freq;
// setting our volume
0.2 => s.gain; 

// chance variable
1 => int chance; 

if( chance == 1 ) 
{
    // sound only play if chance == 1   
    1::second => now; 
}