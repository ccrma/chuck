// strops2.ck: more string operations
//
// for string API documentation, see:
// https://chuck.stanford.edu/doc/reference/base.html#string

// a string
"the quick brown fox jumped over the lazy dog" @=> string str;

// print string
<<< "str:", str >>>;

// read the first char (as int)
<<< "str.charAt( 0 ):", str.charAt( 0 ) >>>;
// read 10th char
<<< "str.charAt( 10 ):", str.charAt( 10 ) >>>;
// test character by index
str.setCharAt( 0, 'T' );
<<< "str.setCharAt( 0, 'T' ):", str >>>;

// test substring -- from the 10th char
<<< "str.substring( 10 ):", str.substring( 10 ) >>>;
// test substring -- from 10th char, for 10 chars
<<< "str.substring( 10, 10 ):", str.substring(10, 10) >>>;

// insert
str.insert( 36, "old " );
<<< "str.insert( 36, \"old \" ):", str >>>;

// test erase
str.erase( 40, 5 );
<<< "str.erase( 40, 5 ):", str >>>;

// test replace
str.replace( 40, "cat" );
<<< "str.replace( 40, \"cat\" ):", str >>>;
// test replace
str.replace( 4, 5, "slow" );
<<< "str.replace( 4, 5, \"slow\" ):", str >>>;

// test replace | 1.5.1.3 or higher
// str.replace( "slow brown", "hungry" );
// <<< "str.replace( \"slow brown\", \"hungry\" ):", str >>>;

// test find -- index of letter, return -1 if not found
<<< "str.find('x'):", str.find('x') >>>;
// test find
<<< "str.find('x', 15):", str.find('x', 15) >>>;
// test find
<<< "str.find(\"fox\"):", str.find("fox") >>>;
// test find
<<< "str.find(\"fox\", 20):", str.find("fox", 20) >>>;

// test rfind
<<< "str.rfind('o'):", str.rfind('o') >>>;
// test rfind
<<< "str.rfind('o', 20):", str.rfind('o', 20) >>>;
// test rfind
<<< "str.rfind(\"fox\"):", str.rfind("fox") >>>;
// test rfind
<<< "str.rfind(\"fox\", 20):", str.rfind("fox", 20) >>>;
