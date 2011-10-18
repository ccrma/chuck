PercFlut flute => JCRev reverb => Gain master => dac;

0 => master.gain;
0 => master.op;

MAUI_View control_view;
MAUI_LED on_led, latch_led;
MAUI_Button on_button, latch;
MAUI_Slider midi_note_slider, volume_slider, reverb_mix_slider;

control_view.size( 200, 325 );

on_button.pushType();
on_button.size( 100, 100 );
on_button.position( 0, 0 );
on_button.name( "noteOn" );
control_view.addElement( on_button );

on_led.size( 100, on_led.height() );
on_led.position( 0, on_button.x() + on_button.height() + 1);
on_led.color( MAUI_LED.blue );
control_view.addElement( on_led );

latch.toggleType();
latch.size( on_button.width(), on_button.height() );
latch.position( on_button.x() + on_button.width(), on_button.y() );
latch.name( "latch" );
control_view.addElement( latch );

midi_note_slider.range( 0, 127 );
midi_note_slider.size( 200, midi_note_slider.height() );
midi_note_slider.position( 0, on_led.height() + on_led.y() );
midi_note_slider.value( 60 );
midi_note_slider.name( "MIDI Note" );
midi_note_slider.displayFormat( MAUI_Slider.integerFormat );
Std.mtof( midi_note_slider.value() ) => flute.freq;
control_view.addElement( midi_note_slider );

volume_slider.range( 0, 1 );
volume_slider.size( 200, volume_slider.height() );
volume_slider.position( 0, midi_note_slider.height() + midi_note_slider.y() );
volume_slider.value( master.gain() );
volume_slider.name( "Gain" );
control_view.addElement( volume_slider );

control_view.display();

function void controlNoteOn()
{
	while( true )
	{
		// wait for the button to be pushed down
		on_button => now;
		on_led.light();
		1 => master.op;
		
		// wait for button to be released
		on_button => now;
		on_led.unlight();
		0 => master.op;
	}
}

function void controlLatch()
{
	while( true )
	{
		// wait for button to be pushed down
		latch => now;
		//on_led.light();
		on_button.toggleType();

		// wait for button to be pushed up
		latch => now;
		//on_led.unlight();
		on_button.pushType();
	}
}

function void controlMIDINote()
{
	while( true )
	{
		midi_note_slider => now;
		Std.mtof( midi_note_slider.value() ) => flute.freq;
	}
}

function void controlGain()
{
	while( true )
	{
		volume_slider => now;
		volume_slider.value() => master.gain;
	}
}

spork ~ controlNoteOn();
spork ~ controlLatch();
spork ~ controlMIDINote();
spork ~ controlGain();

while( true )
	1::day => now;
