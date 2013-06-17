// sound chain
SinOsc s => dac; 
// setting frequency
222.45 => s.freq;
// setting our volume 
0.2 => s.gain; 

// chance variable
3 => int chance; 

if (chance == 1) 
{
    // Sound only play if chance == 1   
    1::second => now; 
}
else 
{
    440.32 => s.freq; // set new freq
    3::second => now; // play sound for 3 second
}