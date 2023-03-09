// negative array index access tests
[1,2,3,4] @=> int a[];

// failure cases
if( a[-1] != 4 ) <<< "FAILURE, expecting 4" >>>;
if( a[-4] != 1 ) <<< "FAILURE, expecting 1" >>>;

// should generate a array out of bounds
a[-5];
