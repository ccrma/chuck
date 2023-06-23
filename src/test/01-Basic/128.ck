// this checks non-empty arrays of references and full instances
// june 2023

// 2x2 array of SinOsc refs
SinOsc @ foo[2][2];

// 2x2 array of SinOsc instances
SinOsc bar[2][2];

// test
if( foo[1][1] == null && bar[1][1] != null ) 
    <<< "success" >>>;
else
    <<< foo[1][1], bar[1][1] >>>;
