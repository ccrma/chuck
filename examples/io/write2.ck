FileIO fout;

// open for write
fout.open( "out2.txt", FileIO.WRITE );

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
