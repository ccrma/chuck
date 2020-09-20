// the very hacked StringTokenizer

// make one
StringTokenizer tok;

// set the string
tok.set( "tokenize me please!" );

// iterate
while( tok.more() )
{
    // print
    <<< tok.next(), "" >>>;
}
