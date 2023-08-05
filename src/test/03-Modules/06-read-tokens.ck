//---------------------------------------------------------------------
// read text tokens
// adapted from examples/io/read-tokens.ck
//---------------------------------------------------------------------

// path and name of file to read
me.dir() + "jabberwocky.txt" => string filename;
// or get filename from command line arguments, if any
if( me.args() > 0 ) me.arg(0) => filename;

// instantiate a file IO object
FileIO fio;
// a string tokenizer
StringTokenizer tokenizer;
// a line of text
string line;
// a word token
string word;

// open the file
fio.open( filename, FileIO.READ );

// ensure it's ok
if( !fio.good() )
{
    cherr <= "can't open file: " <= filename <= " for reading..."
          <= IO.newline();
    me.exit();
}

// new line
cherr <= IO.newline();

// loop until end
while( fio.more() )
{
    // read current line
    fio.readLine() => line;
    // for each line, tokenize
    tokenizer.set( line );
    // loop over tokens
    while( tokenizer.more() )
    {
        // get the next word
        tokenizer.next() => word;
        // print word followed by a space
        cherr <= word <= " ";
    }
    // done with current line
    cherr <= IO.newline();
}
