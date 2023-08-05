// filename
me.dir() + "09-write-output.txt" => string filename;

// instantiate a file IO object
FileIO f;
// open for write (default mode: ASCII)
f.open( filename, FileIO.WRITE );
// test
if( !f.good() )
{
    cherr <= "can't open file for writing..." <= IO.newline();
    me.exit();
}
// write some stuff
f.write( 1 );
f.write( 2 );
f.write( "boo" );
// close the thing
f.close();

// open for write (default mode: ASCII)
f.open( filename, FileIO.READ );
// test
if( !f.good() )
{
    cherr <= "can't open file for reading..." <= IO.newline();
    me.exit();
}
// read
f => string z;
// close the thing
f.close();

// verify
if( z == "12boo" )
    <<< "success" >>>;
