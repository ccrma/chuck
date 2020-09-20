// open a file
IO.openFile( "foo.txt", "r" ) @=> File fin;

// check if it's ok
if( !fin.good() ) { /* do something */ }
// checking eof
while( !fin.eof() ) { /* do something */ }
// alternately
while( fin.more() ) { /* do something */ }

// set position
0 => fin.pos;
// read line
fin.readline() @=> string line;
// read next string (delimeted by white space)
fin.readString() @=> string token;
// read next int
fin.readInt() @=> int i;
// read next float
fin.readFloat() @=> float f;

fin >> token >> i >> f;
fin -> token -> i -> f;
fin --> token --> i --> f;

// cin :) line :) 3;
// cout :( "foo" :( 3 :( EOL;

// read 32-bit binary (same as readInt())
fin.read32i() @=> i;
// read 24-bit binary into 3 LSB of int
fin.read24i() @=> i;
// read 16-bit binary into 2 LSB of int
fin.read16i() @=> i;
// read 8-bit binary
fin.read8i() @=> i;
// read single float binary, stored in chuck float
fin.read32f() @=> f;
// read double float binary (same as readFloat())
fin.read64f() @=> f;

// the array version of the above
fin.read32f( fvals, amount ) @=> int amountRead;
