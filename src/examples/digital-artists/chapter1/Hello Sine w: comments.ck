// Author: Ajay Kapur
// Date: Today's date

// sound chain
SinOsc s => dac; 

// prints out program name
<<<"Hello Sine!">>>; 

// set volume to .6
0.6 => s.gain; 
// Set frequency to 220
220 => s.freq; 
// play for 1 second
1::second => now; 

0.5 => s.gain; // Set Volume to .5 
440 => s.freq;  // Set frequency to 440
2::second => now; // play for 2 seconds

// commented out for now
/*
.3 => s.gain; 
330 => s.freq; 
3::second => now; 
*/

