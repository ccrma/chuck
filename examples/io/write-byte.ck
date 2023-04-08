//---------------------------------------------------------------------
// name: write-byte.ck
// desc: example of writing bytes in BINARY mode
//---------------------------------------------------------------------
// filename
"write-byte.txt" => string filename;
// instantiate a file IO object
FileIO fout;
// open for write (in BINARY mode)
fout.open( filename, FileIO.WRITE | FileIO.BINARY );

// test
if( !fout.good() )
{
    cherr <= "can't open file for writing..." <= IO.newline();
    me.exit();
}

// byte sequence to write; equivalent to:
// abc\n123\n
[97, 98, 99, 10, 49, 50, 51, 10] @=> int bytes[];

// loop
for( int i; i < bytes.size(); i++ )
{
    // write the next byte
    // (other options include IO.INT16 and IO.INT32)
    fout.write( bytes[i], IO.INT8 );
}

// close the thing
fout.close();

// print
<<< "file written:", filename >>>;
