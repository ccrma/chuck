// default file
me.sourceDir() + "/str.txt" => string filename;

// look at command line
if( me.args() > 0 ) me.arg(0) => filename;

// instantiate
FileIO fio;

// open a file
fio.open( filename, FileIO.READ );

// ensure it's ok
if( !fio.good() )
{
    cherr <= "can't open file: " <= filename <= " for reading..." <= IO.nl();
    me.exit();
}

// variable to read into
string str;

// loop until end
while( fio => str )
{
    chout <= str <= IO.newline();
}
