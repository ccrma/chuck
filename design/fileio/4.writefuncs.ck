// open a file
IO.openFile( "foo.txt", "w" ) @=> File fout;

// check if it's ok
if( !fout.good() ) { /* do something */ }
// checking eof
while( !fout.eof() ) { /* do something */ }
// alternately
while( fout.more() ) { /* do something */ }

// set position
0 => fout.pos;
// write line
fout.writeline( "this is a line" );
// write (string)
fout.write( "foo" );
// write (int)
fout.write( 2 );
// write (float)
fout.write( 2.3 );
// write int
fout.writeInt( 2 );
// write float
fout.writeFloat( 1.0 );
// write string
fout.writeString( "foo" );

fout << "foo" << 3 << 2.0 << EOL;
fout <- "foo" <- 3 <- 2.0 <- EOL;
fout <-- "foo" <-- 3 <-- 2.0 <-- EOL;
<fout> "foo" : 3 : 2.0 : EOL;

// write 32-bit binary (same as write(int))
fout.write32i( i );
// write 24-bit binary from 3 LSB of int
fout.write24i( i );
// write 16-bit binary from 2 LSB of int
fout.write16i( i );
// write 8-bit binary
fout.write8i( i );
// write single float binary
fout.write32f( f );
// write double float binary
fout.write64f( f );

// the array version of the above
fout.write32f( fvals, amount ) @=> int amountWritten;
