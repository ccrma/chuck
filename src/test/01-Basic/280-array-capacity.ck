// unit test to make sure that setting capacity < size does not
// alter size or any within-size array elements

// array
[1,2,3,4,5] @=> int x[];
// current size
<<< "size:", x.size() >>>;
// set capacity less than size
2 => x.capacity;
// size (should not change)
<<< "size:", x.size() >>>;
// contents should be intact
for( int a : x ) <<< a >>>;
