// name: mand-o-ui.ck
// desc: STK mandolin demo using MAUI elements
//       (run in miniAudicle)
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Spencer Salazar (ssalazar@cs.princeton.edu)
//         Ananya Misra (amisra@cs.princeton.edu)


// our patch
Mandolin m => JCRev reverb => Gain master => dac;

// set master gain
.75 => master.gain;
.1 => reverb.mix;

// maui elements
MAUI_View control_view;
MAUI_LED on_led; MAUI_LED latch_led;
MAUI_Button on_button; MAUI_Button latch;
MAUI_Slider midi_note_slider;
MAUI_Slider volume_slider;
MAUI_Slider reverb_mix_slider;
MAUI_Slider body_size_slider;
MAUI_Slider pluck_pos_slider;
MAUI_Slider damping_slider;
MAUI_Slider detune_slider;

// set view
control_view.size( 400, 275 );
control_view.name( "Real-time Mandolin Model" );

// button (noteOn)
on_button.pushType();
on_button.size( 100, 80 );
on_button.position( 0, 0 );
on_button.name( "pluck!!" );
control_view.addElement( on_button );

// led
on_led.size( 100, on_led.height() );
on_led.position( 200, 8 );
on_led.unlight();
control_view.addElement( on_led );
latch_led.size( 100, on_led.height() );
latch_led.position( 300, 8 );
latch_led.light();
control_view.addElement( latch_led );

// latch button
latch.toggleType();
latch.size( on_button.width(), on_button.height() );
latch.position( on_button.x() + on_button.width(), on_button.y() );
latch.name( "latch" );
control_view.addElement( latch );

// note
midi_note_slider.range( 30, 127 );
midi_note_slider.size( 200, midi_note_slider.height() );
midi_note_slider.position( 0, 200 );
midi_note_slider.value( 60 );
midi_note_slider.name( "Note" );
Std.mtof( midi_note_slider.value() ) => m.freq;
control_view.addElement( midi_note_slider );

// volume
volume_slider.range( 0, 1 );
volume_slider.size( 200, volume_slider.height() );
volume_slider.position( 200, 200 );
volume_slider.value( master.gain() );
volume_slider.name( "Gain" );
control_view.addElement( volume_slider );

// body size
body_size_slider.range( 0, 1 );
body_size_slider.size( 200, body_size_slider.height() );
body_size_slider.position( 0, 135 );
body_size_slider.value( master.gain() );
body_size_slider.name( "body size" );
control_view.addElement( body_size_slider );

// pluck pos
pluck_pos_slider.range( 0, 1 );
pluck_pos_slider.size( 200, volume_slider.height() );
pluck_pos_slider.position( 200, 135 );
pluck_pos_slider.value( master.gain() );
pluck_pos_slider.name( "pluck position" );
control_view.addElement( pluck_pos_slider );

// damping
damping_slider.range( 0, 1 );
damping_slider.size( 200, volume_slider.height() );
damping_slider.position( 0, 70 );
damping_slider.value( master.gain() );
damping_slider.name( "string damping" );
control_view.addElement( damping_slider );

// detune
detune_slider.range( 0, 1 );
detune_slider.size( 200, volume_slider.height() );
detune_slider.position( 200, 70 );
detune_slider.value( master.gain() );
detune_slider.name( "string detune" );
control_view.addElement( detune_slider );

control_view.display();

function void controlNoteOn()
{
	while( true )
	{
		// wait for the button to be pushed down
		on_button => now;
		on_led.light();
		1 => m.noteOn;
		
		// wait for button to be released
		on_button => now;
		on_led.unlight();
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
		Std.mtof( midi_note_slider.value() ) => m.freq;
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

function void controlBodySize()
{
	while( true )
	{
		body_size_slider => now;
		body_size_slider.value() => m.bodySize;
	}
}

function void controlPluckPos()
{
	while( true )
	{
		pluck_pos_slider => now;
		pluck_pos_slider.value() => m.pluckPos;
	}
}

function void controlDamping()
{
	while( true )
	{
		damping_slider => now;
		damping_slider.value() => m.stringDamping;
	}
}

function void controlDetune()
{
	while( true )
	{
		detune_slider => now;
		detune_slider.value() => m.stringDetune;
	}
}

spork ~ controlNoteOn();
spork ~ controlLatch();
spork ~ controlMIDINote();
spork ~ controlGain();
spork ~ controlBodySize();
spork ~ controlPluckPos();
spork ~ controlDamping();
spork ~ controlDetune();

while( true )
	1::day => now;
