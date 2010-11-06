// instantiate a Perry
PRC tokens;

// set the tokens
tokens.set( "a b c 3.343 2.3" );

// loop over tokens
while( tokens.more() )
{
    // print
    <<< tokens.next() >>>;
}
