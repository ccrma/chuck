
CNoise noise => Pan4 pan => dac;

"pink" => noise.mode;
0.1 => noise.gain;

while(true)
{
    pan.pan() + 0.01 => pan.pan;
    10::ms => now;
}
