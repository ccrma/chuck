//---------------------------------------------------------------------
// name: read-line.ck
// desc: basic example of reading a line of text using FileIO
//---------------------------------------------------------------------

// default file
me.dir() + "04-read-line-data.txt" => string filename;

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

// loop until end
while( fio.more() )
{
    cherr <= fio.readLine() <= IO.newline();
}
