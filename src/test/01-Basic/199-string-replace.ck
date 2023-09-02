// replace all occurances of a substring with a different string
// (requires chuck-1.5.1.3 or higher) @nshaheed

// a string
"Hello, the world! Hello, the world!" => string str;

// replace instances of "the world" with "ChucK"
str.replace( "the world", "ChucK" );
// et voila
<<< str >>>;

// keep on replacing
str.replace( ", ChucK! Hello, ChucK!", " the world" );
str.replace( "H", "h" );
// print
<<< str >>>;

// replace starting at position 6
str.replace( 6, "cat" );
// et voila
<<< str >>>;

// string.replace( position, length, str )
for( int x : Std.range(0,10) )
{
    // reset
    str => string str2;
    // replace
    str2.replace( 6, x, "kitty" );
    // print
    <<< "replace( 6,", x, "):", str2 >>>;
}

// for string API documentation, see:
// https://chuck.stanford.edu/doc/reference/base.html#string
