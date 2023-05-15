//----------------------------------------------------
// name: StringTokenizer
// desc: Break a string into tokens using a whitespace delimiter
//       Iterate through the tokens using next() + more() 
//       or get(i) to get the i-th token
//
// author: terry feng
//----------------------------------------------------

StringTokenizer strtok;

// set the string
strtok.set( "Tokenize me please!" );

// check how many tokens there are
<<< "tokens found:", strtok.size() >>>;

// iterate through the tokens
while( strtok.more() )
{
    // print current token
    <<< strtok.next(), "" >>>;
}

// reset the tokenizer
strtok.reset();

string foo;

// get the first token and pass it to foo
strtok.next( foo );
<<< "first token:", foo >>>;

// another way to get the first token
<<< "first token:", strtok.get( 0 ) >>>;

// get last token and pass it to foo
strtok.get( strtok.size() - 1, foo );
<<< "last token:", foo >>>;
