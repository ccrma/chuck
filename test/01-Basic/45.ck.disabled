// 45.ck : midi event and writing to file (fail-correct)
sinosc s => dac;

MidiIn min;
MidiMsg msg;
MidiRW mrw;

// open MIDI device - see 'chuck --probe'
if( !min.open( 0 ) )
    me.exit();

// open
mrw.open( "y.txt" );

// infinite time loop
while( true )
{
    // wait on event
    min => now;

    // loop through msg
    while( min.recv( msg ) )
    {
	if( msg.data1 == 144 ) // note on
	{
            <<<"now: ", now, "msg: ", msg.data1, msg.data2, msg.data3>>>;
	    std.mtof(msg.data2) => s.freq;
	    msg.data3 / 128.0 => s.gain;
	    mrw.write( msg, now );
	}
    }
}

