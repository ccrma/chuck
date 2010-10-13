// open a file
IO.openFile( "foo.txt", "w" ) @=> File fout;

// check if it's ok
if( !fout.good() )
{ /* do something */ }

// declare some stuff
// fields
string name;
float fval;
int ival;

// loop over for read, ascii
while( /* has more data */ )
{
    // write a string as line
    fout.writeline( "foo: " + 3 + dac );
    // write the same line via ->
    fout -> "foo:" -> 3 -> dac -> EOL;

    // write as binary
    fout.write8( [1,2,3] );
    fout.write16( [1,2,3] );
    fout.write24( [1,2,3] );
    fout.write32( [1,2,3] );
    fout.writeSingle( [1.0,2.0,3.0] );
    fout.writeDouble( [1.0,2.0,3.0] );    
}
