//---------------------------------------------------------------------
// name: read-int.ck
// desc: basic example of reading int using FileIO
//---------------------------------------------------------------------

// default file
me.dir() + "03-read-int-data.txt" => string filename;

// look at command line
if( me.args() > 0 ) me.arg(0) => filename;

// instantiate
FileIO fio;

// open a file
fio.open( filename, FileIO.READ );

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
while( fio => val )
{
    cherr <= val <= IO.newline();
}
