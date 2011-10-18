// name: mode-o-ui.ck
// desc: modal bar demo using MAUI elements
//       (need miniAudicle)
//
// author: Ananya Misra (amisra@cs.princeton.edu)
//         Ge Wang (gewang@cs.princeton.edu)
//         Spencer Salazar (ssalazar@cs.princeton.edu)


// our patch
ModalBar m => JCRev reverb => Gain master => dac;

// set master gain
.75 => master.gain;
.1 => reverb.mix;

// maui elements
MAUI_View control_view;
MAUI_LED on_led;
MAUI_Button on_button; MAUI_Button latch;
MAUI_Slider midi_note_slider;
MAUI_Slider volume_slider;
MAUI_Slider reverb_mix_slider;
MAUI_Slider strike_vigor_slider;
MAUI_Slider strike_pos_slider;
MAUI_Slider stick_hardness_slider;
MAUI_Slider stick_mix_slider;

// array of buttons
MAUI_Button buttons[9];

// set view
control_view.size( 420, 450 );
control_view.name( "Real-time ModalBar Model" );

// button (noteOn)
on_button.pushType();
on_button.size( 125, 75 );
on_button.position( 0, 0 );
on_button.name( "on!!" );
control_view.addElement( on_button );

// led
on_led.size( 100, on_led.height() );
on_led.position( 215, 8 );
on_led.unlight();
control_view.addElement( on_led );

// latch button
latch.toggleType();
latch.size( on_button.width(), on_button.height() );
latch.position( on_button.x() + 100, on_button.y() );
latch.name( "crash" );
control_view.addElement( latch );

// presets
for( 0 => int b; b < 9; b++ )
{
	buttons[b].toggleType();
	buttons[b].size( 125, 75 );
}

// position
70 => int y;

// marimba
buttons[0].position( 0, y );
buttons[0].name( "marimba" );
control_view.addElement( buttons[0] );

// vibraphone
buttons[1].position( 100, y );
buttons[1].name( "vibraphone" );
control_view.addElement( buttons[1] );

// agogo
buttons[2].position( 200, y );
buttons[2].name( "agogo" );
control_view.addElement( buttons[2] );

// wood1
buttons[3].position( 300, y );
buttons[3].name( "wood1" );
control_view.addElement( buttons[3] );

// change y
50 +=> y;

// reso
buttons[4].position( 0, y );
buttons[4].name( "reso" );
control_view.addElement( buttons[4] );

// wood2
buttons[5].position( 100, y );
buttons[5].name( "wood2" );
control_view.addElement( buttons[5] );

// beats
buttons[6].position( 200, y );
buttons[6].name( "beats" );
control_view.addElement( buttons[6] );

// clump
buttons[8].position( 300, y );
buttons[8].name( "clump" );
control_view.addElement( buttons[8] );

// two fixed
buttons[7].position( 0, y + 50 );
buttons[7].name( "two fixed" );
control_view.addElement( buttons[7] );

// note
midi_note_slider.range( 30, 127 );
midi_note_slider.size( 200, midi_note_slider.height() );
midi_note_slider.position( 0, 370 );
midi_note_slider.value( 60 );
midi_note_slider.name( "Note" );
Std.mtof( midi_note_slider.value() ) => m.freq;
control_view.addElement( midi_note_slider );

// sliders
// volume
volume_slider.range( 0, 1 );
volume_slider.size( 200, volume_slider.height() );
volume_slider.position( 200, 370 );
volume_slider.value( master.gain() );
volume_slider.name( "Gain" );
control_view.addElement( volume_slider );

// strike_vigor (strike)
strike_vigor_slider.range( 0, 1 );
strike_vigor_slider.size( 200, strike_vigor_slider.height() );
strike_vigor_slider.position( 0, 305 );
strike_vigor_slider.value( master.gain() );
strike_vigor_slider.name( "strike vigor" );
control_view.addElement( strike_vigor_slider );

// strike pos
strike_pos_slider.range( 0, 1 );
strike_pos_slider.size( 200, strike_pos_slider.height() );
strike_pos_slider.position( 200, 305 );
strike_pos_slider.value( master.gain() );
strike_pos_slider.name( "strike position" );
control_view.addElement( strike_pos_slider );

// stick hardness
stick_hardness_slider.range( 0, 1 );
stick_hardness_slider.size( 200, stick_hardness_slider.height() );
stick_hardness_slider.position( 0, 240 );
stick_hardness_slider.value( master.gain() );
stick_hardness_slider.name( "stick hardness" );
control_view.addElement( stick_hardness_slider );

// stick mix (direct gain)
stick_mix_slider.range( 0, 1 );
stick_mix_slider.size( 200, stick_mix_slider.height() );
stick_mix_slider.position( 200, 240 );
stick_mix_slider.value( master.gain() );
stick_mix_slider.name( "direct stick mix" );
control_view.addElement( stick_mix_slider );

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

		// secret call
		Machine.crash();
	}
}

function void controlButton( int index )
{
	while( true )
	{
		// wait for the button to be pushed down
		buttons[index] => now;
		
		// change preset
		index => m.preset;

		// change other buttons
		for( 0 => int i; i < 9; i++ )
		{
			if( i != index )
			{	
				// hack
				buttons[i].pushType();
				buttons[i].toggleType();
			}
		}		

		// wait for button to be released
		buttons[index] => now;
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

function void controlStrikeVigor()
{
	while( true )
	{
		strike_vigor_slider => now;
		strike_vigor_slider.value() => m.strike;
	}
}

function void controlStrikePos()
{
	while( true )
	{
		strike_pos_slider => now;
		strike_pos_slider.value() => m.strikePosition;
	}
}

function void controlStickHardness()
{
	while( true )
	{
		stick_hardness_slider => now;
		stick_hardness_slider.value() => m.stickHardness;
	}
}

function void controlStickMix()
{
	while( true )
	{
		stick_mix_slider => now;
		stick_mix_slider.value() => m.directGain;
	}
}

spork ~ controlNoteOn();
spork ~ controlLatch();
spork ~ controlMIDINote();
spork ~ controlGain();
spork ~ controlStrikeVigor();
spork ~ controlStrikePos();
spork ~ controlStickHardness();
spork ~ controlStickMix();
for( 0 => int i; i < 9; i++ )
	spork ~ controlButton( i );

while( true )
	1::day => now;
