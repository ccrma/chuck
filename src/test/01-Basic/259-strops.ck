// strops.ck some basic string operations
//
// for string API documentation, see:
// https://chuck.stanford.edu/doc/reference/base.html#string

// three strings
"hello" => string foo;
"hello" => string bar;
"there" => string geh;

// makeshift assert
fun void assert( int condition, string text )
{
    if( !condition )
    {
        <<< "assertion failed: ", text >>>;
        me.exit();
    }
}

// equality
assert( foo == foo, "1" );
assert( foo == bar, "2" );
assert( "abc" == "abc", "3" );
assert( "hello" == foo, "4" );

assert( foo != geh, "5" );
assert( "x" != "y", "6" );
assert( foo != "there", "7" );

// lt
assert( foo < geh, "8" );
assert( foo < "hello!", "9" );
assert( foo <= foo, "10" );
assert( foo <= "there", "11" );

// gt
assert( foo > "abc", "12" );
assert( foo > "b", "13" );
assert( foo >= foo, "14" );
assert( foo >= bar, "15" );

// concatenation
assert( "foo" + "bar" == "foobar", "16" );
"foo" => string s;
"bar" +=> s;
assert( s == "foobar", "17" );
assert( "bar" + 10 == "bar10", "18" );
// "bar10.000000" -- using find in case # of 0s after decimal differs
assert( ("bar" + 10.0).find("bar10.") == 0, "19" );
assert( 10 + "bar" == "10bar", "20" );
// "10.000000bar" -- using find in case # of 0s after decimal differs
assert( (10.0 + "bar").find("bar") > 2, "21" );
assert( "foo" + "bar" + "cle" == "foobarcle", "22" );
assert( "FoO".lower() == "foo", "23" );
assert( "foo".upper() == "FOO", "24" );
assert( " foo  ".trim() == "foo", "25" );
assert( " foo ".ltrim() == "foo ", "26" );
assert( " foo ".rtrim() == " foo", "27" );

<<< "success" >>>;
