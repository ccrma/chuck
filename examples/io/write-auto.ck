// example showing auto-naming for file writing

// instantiate a file IO object
FileIO fout;

// optional: set prefix and extension
// (defaults prefix: "chuck-file" ext: "txt")
fout.autoPrefixExtension( "chuck-file", "txt" );

// open for write (default mode: ASCII)
fout.open( "special:auto", FileIO.WRITE );

// test
if( !fout.good() )
{
    cherr <= "can't open file for writing..." <= IO.newline();
    me.exit();
}

// write some stuff
fout <= 1 <= " " <= 2 <= " " <= "foo" <= IO.newline();

// print the file that was written
// FYI before we close it as that clears the filename
<<< "file written:", fout.filename() >>>;

// close the thing
fout.close();
