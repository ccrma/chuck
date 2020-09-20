// 38.ck : midi (fail-correct)	

MidiOut mout;
MidiMsg msg;

// open (see 'chuck --probe')
if( !mout.open( 0 ) )
{
    <<<"bad">>>;
    me.exit();
}

// infinite time loop
while( true )
{
    // make MIDI msg
    144 => msg.data1;
    std.rand2( 10, 80 ) => msg.data2;
    std.rand2( 40, 100 ) => msg.data3;

    // send it
    mout.send( msg );

    // wait
    100::ms => now;
}

