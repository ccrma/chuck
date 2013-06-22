// sound chain: white noise to pan to dac
Noise n => Pan2 p => dac;

// infinite loop
while( true )
{
    Math.sin(now/1::second*2*pi) => p.pan; 
    10::ms => now; 
}

