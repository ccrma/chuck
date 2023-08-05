// test binary write + read
me.dir() + "01-file-io.bin" => string filename;

// writer
FileIO f;
f.open( filename, FileIO.BINARY | FileIO.WRITE );
// read
f.write(1, IO.INT16);
f.write(2, IO.INT16);
f.write(3, IO.INT16);
// close
f.close();

// reader
FileIO g;
g.open( filename, FileIO.BINARY | FileIO.READ );
// ensure it's ok
if( !g.good() )
{
    cherr <= "can't open file: for reading..." <= IO.newline();
    me.exit();
}
// read
g.readInt( IO.INT16 ) => int x;
g.readInt( IO.INT16 ) => int y;
g.readInt( IO.INT16 ) => int z;
// close
g.close();

// verify
if( x == 1 && y == 2 && z == 3 )
    <<< "success" >>>;
