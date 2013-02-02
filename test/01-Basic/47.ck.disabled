// 47.ck : midi write (fail-correct)

sinosc s => dac;

MidiIn min;
MidiMsg msg;
MidiMsgOut mrw;  // use MidiMsgOut

// open MIDI device
if( !min.open( 0 ) )
    me.exit();

// open for write
mrw.open( "z.txt" );

int count;

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
            <<< "now:", now, msg.data1, msg.data2, msg.data3 >>>;
            std.mtof(msg.data2) => s.freq;
            msg.data3 / 128.0 => s.gain;
            mrw.write( msg, now );
            //<<<count + 1 => count>>>;
	}
    }
}

