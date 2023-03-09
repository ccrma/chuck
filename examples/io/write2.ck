// filename
"write2.txt" => string filename;
// instantiate a file IO object
FileIO fout;
// open for write (default mode: ASCII)
fout.open( filename, FileIO.WRITE );

// test
if( !fout.good() )
{
    cherr <= "can't open file for writing..." <= IO.newline();
    me.exit();
}

// write some stuff
fout.write( 1 );
fout.write( 2 );
fout.write( "boo" );

// close the thing
fout.close();

// print
<<< "file written:", filename >>>;
