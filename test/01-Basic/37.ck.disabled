// 37.ck : midi (fail-correct)

MidiIn min;
MidiMsg msg;

// open (see 'chuck --probe')
if( !min.open( 0 ) )
{
    <<<"bad">>>;
    me.exit();
}

// infinite time loop
while( true )
{
    // poll (also see alternative to polling: events)
    while( min.recv( msg ) )
    {
        <<<msg.data3>>>;
    }

    // wait
    5::ms => now;
}

