// sound chain
SinOsc s => Pan2 p => dac;

// initialize pan Position value 
1.0 => float panPosition; 

// loop
while( panPosition > -1.0 )
{
    panPosition => p.pan; // pan value (between 1.0 and -1.0)
    <<< panPosition >>>; // print Value
    panPosition - .01 => panPosition; // move pan position value
    .01:: second => now; // advance time
}

