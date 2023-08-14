// an array initializer list (aka array literal)
[1,2,3] @=> int foo[];

// also permitted: trailing comma after last element in array literal
[1,2,3,] @=> int bar[];

// print
if( foo.size() == 3 && bar.size() == 3 )
    <<< "success" >>>;
