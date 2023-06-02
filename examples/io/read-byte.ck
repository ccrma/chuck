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
    fio.readInt( IO.INT8 ) => val;
    // optionsss ^^^^^^^ | requires chuck-1.5.0.1 or higher
    // IO.INT8, IO.INT16, IO.INT32 (default to unsigned)
    // IO.UINT8, IO.UINT16, IO.UINT32 (explicitly unsigned)
    // IO.SINT6, IO.SINT16, IO.SINT32 (explicitly signed)

    // print the value
    cherr <= val <= IO.newline();
}
