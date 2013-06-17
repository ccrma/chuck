// sound chain
SinOsc s => dac; 

// middle C freq in hertz
261.63 => float myfreq;
// variable for volume control
.6 => float myVol; 

// set frequency
myfreq => s.freq;
// set volume  
myVol => s.gain; 
// 1 second duration
1::second => now; 