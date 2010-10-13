// open a file
IO.openFile( "foo.txt", "r" ) @=> File fin;
IO.openFile( "foo.txt", IO.READ | OI.WRITE | OI.APPEND ) @=> File fin;

// check if it's ok
if( !fin.good() )
{ /* do something */ }

// declare some stuff
string line;
StringTokenizer strtok;
// fields
string name;
float fval;
int ival;

// loop over for read, ascii
while( fin.more() )
{
    // read, tokenize line
    strtok.set( fin.readline() );
    // read into typed variables
    strtok => name => ival => fval;
}
