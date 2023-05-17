
CNoise noise => AmbPan3 pan => dac;

0.1 => noise.gain;
"pink" => noise.mode;

// W  X  Y  Z  R   S   T  U  V  K  L  M   N   O  P  Q
  [0, 1, 2, 7, 8,  9, 10, 3, 4,11,12,13, 14, 15, 5, 6] => pan.channelMap;

0 => pan.elevation;
pi/2 => pan.azimuth;

while(true)
{
    //pan.azimuth()+pi/1024 => pan.azimuth;
    pan.elevation() + pi/1024 => pan.elevation;
    5::ms => now;
}

