// name: map.ck
// desc: mapping a number in one range to a second range, using
//       Math.map( float value, float x1, float x2,
//                              float y1, float y2 )
//
// version: need chuck-1.5.0.0 or higher
//
// NOTE: Math.map(...) allows 'value' to be outside range [x1,x2]
//       whereas Math.map2(...) clamps 'value' to range [x1,x2]
//
// uncomment this line to print Math library API
// Math.help();
//
// author: Ge Wang
// date: Spring 2023

// map 2.5 from [2,4] to [0,1]
<<< Math.map( 2.5, 2, 4, 0, 1 ), "" >>>;
// map 5 from [1,10] to [-1,1]
<<< Math.map( 5, 1, 10, -1, 1 ), "" >>>;
// map 5 from [10,1] to [-1,1]
<<< Math.map( 5, 10, 1, -1, 1 ), "" >>>;
// map 1 from [2,3] to [0,100]
<<< Math.map( 1, 2, 3, 0, 100 ), "(not clamped)" >>>;
// clampf() and map()
<<< Math.map2( Math.clampf(1,2,3), 2, 3, 0, 100 ), "(clamped using Math.clampf())" >>>;
// same as Math.map2()
<<< Math.map2( 1, 2, 3, 0, 100 ), "(clamped using Math.map2())" >>>;
