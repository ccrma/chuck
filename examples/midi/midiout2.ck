//-----------------------------------------------------------------------------
// name: midiout2.ck
// desc: example of sending MIDI messages using MidiOut only
// note: for a good explanation of how MIDI messages work and sending raw midi
//       messages, see midiout.ck
//-----------------------------------------------------------------------------

// instantiate a MIDI out object
MidiOut mout;
// open a MIDI device for output
if( !mout.open(0) ) me.exit();

<<< "MIDI output device opened...", "" >>>;

// loop
while( true )
{
    <<< "sending note on message...", "" >>>;
    mout.noteOn( 1, 60, 127 );
    1::second => now;

    <<< "sending note off message...", "" >>>;
    mout.noteOff( 1, 60, 0 );
    1::second => now;

    <<< "sending control change message...", "" >>>;
    mout.controlChange( 1, 14, 127 );
    1::second => now;

    <<< "sending program change message...", "" >>>;
    mout.programChange( 1, 6 );
    1::second => now;

    <<< "sending polyphonic key pressure (aftertouch)...", "" >>>;
    mout.polyPressure( 1, 60, 127 );
    1::second => now;

    <<< "sending channel pressure (aftertouch)...", "" >>>;
    mout.channelPressure( 1, 127 );
    1::second => now;

    <<< "sending pitch bend message...", "" >>>;
    mout.pitchBend( 1, 96 ); // +50% or 1.00 semitone up
    1::second => now;

    <<< "sending fine resolution pitch bend message...", "" >>>;
    mout.pitchBend( 1, 50, 96 ); // +51% or 1.01 semitone up
    1::second => now;
}
