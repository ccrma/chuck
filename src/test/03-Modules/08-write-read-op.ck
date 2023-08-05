// filename
me.dir() + "08-write-read-op.txt" => string filename;
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
fout <= 1 <= " " <= 2 <= " " <= "foo" <= IO.newline();
// close the thing
fout.close();

// instantiate a file IO object
FileIO fin;
// open for read (default mode: ASCII)
fin.open( filename, FileIO.READ );
// test
if( !fin.good() )
{
    cherr <= "can't open file for reading..." <= IO.newline();
    me.exit();
}
// read in
fin => int x => int y => string z;
// close the thing
fin.close();

// verify
if( x == 1 && y == 2 && z == "foo" )
    <<< "success" >>>;
