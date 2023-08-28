// added by machine-help.ck

// print
<<< "*** hello! I'm TEST 2 ***", "" >>>;

// print arguments
for( int i; i < me.args(); i++ )
{
    <<< "--- got argument ->", me.arg(i) >>>;
}

// wait
while( true )
{
    1::second => now;
}