//----------------------------------------------------------------------------
// name: midi2osc-s.ck
// desc: starter code for sending MIDI input out to OSC
//
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: Spring 2024
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// global values
//----------------------------------------------------------------------------
// destination host | 224.0.0.1 == multicast
"localhost" => string HOSTNAME;
// destination port number
6449 => int PORT;
// accept MIDI input from up to this number of devices
16 => int NUM_DEVICES;
// OSC transmitter
OscOut XMIT;


//----------------------------------------------------------------------------
// initialization routines
//----------------------------------------------------------------------------
// initialize MIDI
if( !initMIDI( NUM_DEVICES ) ) me.exit();
// initialize OSC
initOSC( XMIT, HOSTNAME, PORT );
//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// infinite time loop
//----------------------------------------------------------------------------
while( true ) 1::second => now;




//----------------------------------------------------------------------------
// initialize OSC
//----------------------------------------------------------------------------
fun void initOSC( OscOut xmit, string hostname, int port )
{
    // aim the transmitter at destination
    xmit.dest( hostname, port );
}




//----------------------------------------------------------------------------
// common OSC sends
//----------------------------------------------------------------------------
// send message with an arbitrary number of floats
fun void send( string address, float values[] )
{
    // start the message...
    XMIT.start( address );
    // loop and add float values
    for( float v : values ) { v => XMIT.add; }
    // send the message
    XMIT.send();
}
// send message with int with an arbitrary number of floats
fun void send( string address, int num, float values[] )
{
    // start the message...
    XMIT.start( address );
    // add int
    num => XMIT.add;
    // loop and add float values
    for( float v : values ) { v => XMIT.add; }
    // send the message
    XMIT.send();
}




//----------------------------------------------------------------------------
// initialize MIDI
//----------------------------------------------------------------------------
fun int initMIDI( int numDevices )
{
    // devices to open (try: chuck --probe)
    MidiIn midi[numDevices];
    
    // opens MIDI input devices one by one, starting from 0, until it
    // reaches one it can't open.  then waits for midi events on all open
    // devices and prints out the device, and contents of the MIDI message
    for( int i; i < midi.size(); i++ )
    {
        // no print err
        midi[i].printerr( 0 );
        
        // open the device
        if( midi[i].open( i ) )
        {
            <<< "device", i, "->", midi[i].name(), "->", "open: SUCCESS" >>>;
            spork ~ midi_go( midi[i], i );
        }
        else
        {
            midi.size( i );
            break;
        }
    }
    
    // check
    if( midi.size() == 0 )
    {
        <<< "(midi2osc-s.ck) no MIDI input devices found, exiting...", "" >>>;
        return false;
    }
    
    return true;
}




//----------------------------------------------------------------------------
// MIDI handler (on per device)
//----------------------------------------------------------------------------
fun void midi_go( MidiIn min, int id )
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
            // print MIDI message with device id (useful for debugging)
            // <<< "MIDI input on device", id, ":", msg.data1, msg.data2, msg.data3 >>>;
 
            // knobs
            if( msg.data1 == 185 )
            {
                send( "/foo/control/knob", msg.data2, [ msg.data3/127.0 ] );
            }
            else if( msg.data1 == 153 )
            {
                send( "/foo/control/button", msg.data2, [ msg.data3/127.0 ] );
            }
        }
    }
}