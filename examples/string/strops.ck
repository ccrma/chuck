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

// concatention
assert( "foo" + "bar" == "foobar", "16" );
"foo" => string s;
"bar" +=> s;
assert( s == "foobar", "17" );
assert( "bar" + 10 == "bar10", "18" );
assert( "bar" + 10.0 == "bar10.0000", "19" );
assert( 10 + "bar" == "10bar", "20" );
assert( 10.0 + "bar" == "10.0000bar", "21" );
assert( "foo" + "bar" + "cle" == "foobarcle", "22" );
assert( "FoO".lower() == "foo", "23" );
assert( "foo".upper() == "FOO", "24" );
assert( " foo  ".trim() == "foo", "25" );
assert( " foo ".ltrim() == "foo ", "26" );
assert( " foo ".rtrim() == " foo", "27" );

<<< "success" >>>;
