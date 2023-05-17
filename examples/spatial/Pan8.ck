
CNoise noise => Pan8 pan => dac;

"pink" => noise.mode;
0.1 => noise.gain;
0 => pan.pan;

while(true)
{
    pan.pan() + 0.01 => pan.pan;
    10::ms => now;
}
