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
assert( 10 + "bar" == "10bar", "19" );
assert( ("bar" + 10.0).find("bar10.") == 0, "20" );
assert( (10.0 + "bar").find("bar") > 2, "21" );
assert( "foo" + "bar" + "cle" == "foobarcle", "22" );
assert( ("foo" + @(3,4)).find("foo@(3.") == 0, "23" );
assert( ("foo" + @(5,6,7)).find("foo@(5.") == 0, "24" );
assert( ("foo" + @(8,9,10)).find("foo@(8.") == 0, "25" );
assert( ("bar" + #(11,12)).find("bar#(11.") == 0, "26" );
assert( ("bar" + @(13,2*pi)).find("bar@(13.") == 0, "27" );
assert( (@(3,4)+"foo").find("foo") > 5, "28" );
assert( (@(5,6,7)+"foo").find("foo") > 6, "29" );
assert( (@(8,9,10)+"foo").find("foo") > 7, "30" );
assert( (#(11,12)+"bar").find("bar") > 5, "31" );
assert( (@(13,2*pi)+"bar").find("bar") > 5, "32" );

// utilities
assert( "FoO".lower() == "foo", "23" );
assert( "foo".upper() == "FOO", "24" );
assert( " foo  ".trim() == "foo", "25" );
assert( " foo ".ltrim() == "foo ", "26" );
assert( " foo ".rtrim() == " foo", "27" );
assert( ("a"+"b"+"c").length() == 3, "28" );

<<< "success" >>>;
