// opens MIDI input devices one by one, starting from 0,
// until it reaches one it can't open.  then waits for
// midi events on all open devices and prints out the
// device, and contents of the MIDI message

// devices to open (try: chuck --probe)
MidiIn min[16];

// number of devices
int devices;

// loop
for( int i; i < min.cap(); i++ )
{
    // no print err
    min[i].printerr( 0 );

    // open the device
    if( min[i].open( i ) )
    {
        <<< "device", i, "->", min[i].name(), "->", "open: SUCCESS" >>>;
        spork ~ go( min[i], i );
        devices++;
    }
    else break;
}

// check
if( devices == 0 )
{
    <<< "um, couldn't open a single MIDI device, bailing out..." >>>;
    me.exit();
}

// infinite time loop
while( true ) 1::second => now;


// handler for one midi event
fun void go( MidiIn min, int id )
{
    // the message
    MidiMsg msg;

    // infinite event loop
    while( true )
    {
        // wait on event
        min => now;

        // print message
        while( min.recv( msg ) )
        {
            // print out midi message with id
            <<< "device", id, ":", msg.data1, msg.data2, msg.data3 >>>;
        }
    }
}
