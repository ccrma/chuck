
// columns
[ 1,2,3,4,5,6,7,8,9 ] @=> int columns[];

// filename
"JaneHappy17.txt" => string filename;

// srate in ms
"10" => string srate;

<<<"setting FILENAME:", filename>>>;
<<<"setting SRATE:", srate>>>;

// set environment
std.setenv( "FILENAME", filename );
std.setenv( "SRATE", srate );

// spork many
for( 0 => int c; c < columns.cap(); c++ )
{
    // set column
    std.setenv( "VCR_COLUMN", std.itoa( columns[c] ) );
    // spork
    machine.add( "yay.ck" );
    // yield
    me.yield();
}
