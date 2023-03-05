// default file
me.sourceDir() + "jabberwocky.txt" => string filename;

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
while( fio.more() )
{
    chout <= fio.readLine() <= IO.newline();
}

<<< "\n------------", "" >>>;
<<< "rewinding...", "" >>>;
<<< "------------\n", "" >>>;
fio.seek( 0 );

// loop until end
while( fio.more() )
{
    chout <= fio.readLine() <= IO.newline();
}


