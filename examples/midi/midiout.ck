// instantiate a MIDI out object
MidiOut mout;
// a message to work with
MidiMsg msg;
// open a MIDI device for output
if( !mout.open(0) ) me.exit();

<<< "MIDI output device opened...", "" >>>;

// loop
while( true )
{
    // MIDI note on message
    // 0x90 + channel (0 in this case)
    0x90 => msg.data1;
    // pitch
    60 => msg.data2;
    // velocity
    127 => msg.data3;
    // print
    <<< "sending NOTE ON message...", "" >>>;
    // send MIDI message
    mout.send( msg );

    // let time pass
    1::second => now;
    
    // MIDI note off message
    // 0x80 + channel (0 in this case)
    0x80 => msg.data1;
    // pitch
    60 => msg.data2;
    // velocity
    0 => msg.data3;
    // print
    <<< "sending NOTE OFF message...", "" >>>;
    // send MIDI message
    mout.send( msg );

    // let time pass
    1::second => now;
}
