FileIO fout;

// open for write
fout.open( "out.txt", FileIO.WRITE );

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
