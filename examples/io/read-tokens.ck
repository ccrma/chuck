// this example shows how to 
// 1) open a text file for reading
// 2) read the file line by line
// 3) for each line, tokenize it into words (separated by spaces)

// default file
me.sourceDir() + "jabberwocky.txt" => string filename;

// look at command line
if( me.args() > 0 ) me.arg(0) => filename;

// instantiate a file IO object
FileIO fio;
// a string tokenizer
StringTokenizer tokenizer;
// a line of text
string line;
// a word token
string word;

// open a file
fio.open( filename, FileIO.READ );

// ensure it's ok
if( !fio.good() )
{
    cherr <= "can't open file: " <= filename <= " for reading..."
    <= IO.newline();
    me.exit();
}

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
        chout <= word <= " ";
    }
    // done with current line
    chout <= IO.newline(); chout.flush();
}
