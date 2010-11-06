// 43.ck : midi event (fail-correct)

sinosc s => DelayL z => dac;

MidiIn min;
MidiMsg msg;

0.0::second => z.delay;
//0.02::second => z.max => z.delay;

// open - see 'chuck --probe'
if( !min.open( 0 ) )
    me.exit();

// inifinite time loop
while( true )
{
    // wait on MIDI event
    min => now;

    // there should be one or more msg if we get here
    while( min.recv( msg ) )
    {
        // print out some data
        <<<msg.data3>>>;
	if( msg.data1 == 144 ) // note on
	{
            <<<msg.data1>>>;
            0.75 * std.mtof(msg.data2) => s.freq;
            msg.data3 / 128.0 => s.gain;
	}
    }
}

