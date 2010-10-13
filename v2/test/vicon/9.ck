
// set column number
9 => int column;
// default srate
10::ms => dur srate;

// get name
std.getenv( "FILENAME" ) => string f;
if( f.length() == 0 )
{
    <<<"set environment variable FILENAME!">>>;
    me.exit();
}

// get srate
std.getenv( "SRATE" ) => string r;
<<<r>>>;
if( r.length() != 0 ) std.atof( r )::ms => srate;

// if srate less than a samp, then something is wrong
if( srate < samp )
{
    <<<"srate:", srate, "too tiny!!!">>>;
    me.exit();
}


// patch
sinosc s => gain g => dac;
.1 => g.gain;


// declare Vicon Column Reader
VCR vcr;

// load file and specify column
if( !vcr.load( f, column ) )
    me.exit();

// print out some info
<<< "-----------------------" >>>;
<<< "file:", f >>>;
<<< "column:", column >>>;
<<< "name:", vcr.name() >>>;
<<< "size:", vcr.size() >>>;
<<< "srate:", srate >>>;


// good
while( vcr.more() )
{
    // yes
    std.abs(vcr.curr()) => s.freq;
    // move to next
    vcr.next();

    // this is srate
    srate => now;
}
