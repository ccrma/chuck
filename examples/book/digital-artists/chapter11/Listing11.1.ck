// Listing 11.1 MIDI output note on/off boilerplate

// MIDI out setup, make a MidiOut object, open it on a device
MidiOut mout;                  // (1) Makes a new MidiOut

// MIDI Port (Window > Device Browser > MIDI > Output)
0 => int port;                 // (2) MIDI port number (varies)

// try to open that port, fail gracefully
if( !mout.open(port) )         // (3) Tries to open the MIDI port
{
    <<< "Error: MIDI port did not open on port: ", port >>>;
    me.exit();
}

// Make a MIDI msg holder for sending
MidiMsg msg;                   // (4) Makes a MIDI message holder

// utility function to send MIDI out notes
fun void MIDInote(int onoff, int note, int velocity)
{                              // (5) Function to send MIDI noteOn/Off
    if (onoff == 0) 128 => msg.data1;  // (6) If noteOff, set status byte to 128...
    else 144 => msg.data1;     // (7) ...else set status  byte to 144.
    note => msg.data2;
    velocity => msg.data3;
    mout.send(msg);
}

// loop
while( true )
{
    Math.random2(60,100) => int note;
    Math.random2(30,127) => int velocity;
    MIDInote(1, note, velocity); // (8) Test noteOn on random note and velocity...
    .1::second => now;
    MIDInote(0, note ,velocity); // (9) ...and test noteOff.
    .1::second => now;
}
