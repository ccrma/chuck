//-----------------------------------------------------------------------------
// name: array_sort.ck
// desc: sort the contents of arrays (various types)
// requires: chuck-1.5.0.9 or higher


//-----------------------------------------------------------------------------
// integer array
//-----------------------------------------------------------------------------
[ 5,2,-1,4,3,1,0,-2 ] @=> int arrayInt[];
// sort by ascending value
arrayInt.sort();
// print
for( auto x : arrayInt ) { cherr <= x <= " "; } cherr <= IO.nl();
// reverse for descending
arrayInt.reverse();
// print
for( auto x : arrayInt ) { cherr <= x <= " "; } cherr <= IO.nl();


//-----------------------------------------------------------------------------
// float array
//-----------------------------------------------------------------------------
[ 2.0, -pi, 1, pi ] @=> float arrayFloat[];
// sort by ascending value
arrayFloat.sort();
// print
for( auto x : arrayFloat ) { cherr <= x <= " "; } cherr <= IO.nl();


//-----------------------------------------------------------------------------
// complex array: sort by 2-norm/mag with phase as tie-breaker
//-----------------------------------------------------------------------------
[ #(-1,-1), #(1,1), #(0,0), #(1,0) ] @=> complex arrayComplex[];
// sort by ascending value
arrayComplex.sort();
// print
for( auto x : arrayComplex ) { cherr <= x <= " "; } cherr <= IO.nl();


//-----------------------------------------------------------------------------
// polar array: sort by magnitude with phase as tie-breaker
//-----------------------------------------------------------------------------
[ %(2,pi/2), %(1,pi), %(2,pi) ] @=> polar arrayPolar[];
// sort by ascending value
arrayPolar.sort();
// print
for( auto x : arrayPolar ) { cherr <= x <= " "; } cherr <= IO.nl();


//-----------------------------------------------------------------------------
// vec3 array: sort by magnitude with x then y then z as tie breakers
//-----------------------------------------------------------------------------
[ @(-1,-1,-1), @(1,2,1), @(0,0,0) ] @=> vec3 arrayVec3[];
// sort by ascending value
arrayVec3.sort();
// print
for( auto x : arrayVec3 ) { cherr <= x <= " "; } cherr <= IO.nl();


//-----------------------------------------------------------------------------
// vec4 array: sort by magnitude with x then y then z then w as tie breakers
//-----------------------------------------------------------------------------
[ @(-1,-1,-1,-1), @(1,3,1,-2), @(0,0,0,0) ] @=> vec4 arrayVec4[];
// sort by ascending value
arrayVec4.sort();
// print
for( auto x : arrayVec4 ) { cherr <= x <= " "; } cherr <= IO.nl();


//-----------------------------------------------------------------------------
// Object array: sort by reference/pointer value for now
//-----------------------------------------------------------------------------
SinOsc foos[3];
// shuffle
foos.shuffle();
// sort by ascending value
foos.sort();
// print
for( auto x : foos ) { <<< x >>>; }
