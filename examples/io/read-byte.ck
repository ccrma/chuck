//---------------------------------------------------------------------
// name: read-byte.ck
// desc: example of reading raw bytes from file
// NOTE: file must be opened in BINARY mode for this to work
//---------------------------------------------------------------------

// default filepath
me.dir() + "read-byte.txt" => string filename;
// look at command line
if( me.args() > 0 ) me.arg(0) => filename;

// instantiate
FileIO fio;
// open a file (in BINARY mode)
fio.open( filename, FileIO.READ | FileIO.BINARY );
// ensure it's ok
if( !fio.good() )
{
    cherr <= "can't open file: " <= filename <= " for reading..."
          <= IO.newline();
    me.exit();
}

// variable to read into
int val;

// loop until end
while( fio.more() )
{
    // read the next byte
    // (other options include IO.INT16 and IO.INT32)
    fio.readInt( IO.INT8 ) => val;

    // print the value
    cherr <= val <= IO.newline();
}
