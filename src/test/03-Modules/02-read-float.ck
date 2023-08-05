//---------------------------------------------------------------------
// name: read-float.ck
// desc: basic example of reading floats using FileIO
//---------------------------------------------------------------------

// default file
me.dir() + "02-read-float-data.txt" => string filename;

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
float val;

// loop until end
while( fio => val )
{
    cherr <= val <= IO.newline();
}
