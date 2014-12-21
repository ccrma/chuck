// Listing 11.2 Simple piano synthesizer controlled by MIDI input

// Setup MIDI input, set a port number, try to open it
MidiIn min;             // (1) Makes a MidiIn object

// MIDI Port (ChucK Menu: Window > Device Browser > MIDI > Input)
0 => int port;

// open the port, fail gracefully
if( !min.open(port) )  // (2) Tries to open it on port, handles failure
{
    <<< "Error: MIDI port did not open on port: ", port >>>;
    me.exit();
}

// holder for received messages
MidiMsg msg;           // (3) Makes object to hold MIDI messages

// make an instrument to play
Rhodey piano => dac;   // (4) Rhodey piano to play with MIDI controller

// loop
while( true )          // (5) Infinite loop
{
    min => now;        // (6) Sleeps until a MIDI input message comes
                       // advance when receive MIDI msg 
    while( min.recv(msg) )
    {
        <<< msg.data1, msg.data2, msg.data3 >>>;
        if (msg.data1 == 144) {
            Std.mtof(msg.data2) => piano.freq;
            msg.data3/127.0 => piano.gain;
            1 => piano.noteOn;
        }
        else {
            1 => piano.noteOff;
        }
    }
}
