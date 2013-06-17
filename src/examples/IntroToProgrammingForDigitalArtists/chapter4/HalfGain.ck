// Sound Network 
SinOsc s => dac;

// Function 
fun float halfGain(float originalGain)
{
    return(originalGain*.5);
}

// Remember that .gain() is a function that is part of SinOsc
<<<"Full Gain: ", s.gain()>>>;
1::second => now;
halfGain(s.gain()) => s.gain;
<<<"Half Gain: ", s.gain()>>>;
1::second => now;

