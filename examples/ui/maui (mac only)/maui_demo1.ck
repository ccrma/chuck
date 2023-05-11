// brief demo of MAUI (MiniAudicle User Interface)
// in this demo, you will see the main features of MAUI (made by Spencer Salazar)
//as documented here: https://audicle.cs.princeton.edu/miniAudicle/mac/doc/maui_api.html


// ______________________General Chuck Sound Stuff______________________________________
// our sound path
SinOsc foo => Gain g => dac;

.5 => foo.gain;

// set the gain of foo to 0 so it doesn't start playing immediately
0 => g.gain;
 
 
//____________________________ MAUI ELEMENTS __________________________________________ 
 

//                        -- initiate elements--

MAUI_View control_view; //call the class into play
MAUI_LED on_led; // create an led light
MAUI_Button on_button; // create a button
MAUI_Slider midi_note_slider, volume_slider; // create two sliders for pitch and volume


control_view.size( 200, 250 ); // size of the canvas
control_view.name( "MAUI Basic-1" ); // the name of the canvas

// create the "on" button
on_button.pushType(); // types can be pushType() or ToggleType() -- TRY IT OUT!!!
on_button.size( 100, 100 ); //size of button
on_button.position( 50, 0 ); //position
on_button.name( "noteOn" ); // name printed on button
control_view.addElement( on_button ); // add the element to control_view canvas

// create the led to go with the on button
on_led.size( 100, on_led.height() );
on_led.position( 50, 75);
on_led.color( MAUI_LED.red ); // can be red, blue or green.
control_view.addElement( on_led );

// create midi number slider
midi_note_slider.range( 0, 127 ); 
midi_note_slider.size( 200, midi_note_slider.height() );
midi_note_slider.position( 0, 120);
midi_note_slider.value( 60 ); // the value where the slider starts at
midi_note_slider.name( "MIDI Note Number" );
midi_note_slider.displayFormat( MAUI_Slider.integerFormat ); // not showing floats
Std.mtof( midi_note_slider.value() ) => foo.freq; // converts midi numbers to frequency
control_view.addElement( midi_note_slider );

// create volume slider for the SinOsc
volume_slider.range( 0, 1 );
volume_slider.size( 200, volume_slider.height() );
volume_slider.position( 0, 170 );
volume_slider.value( .5 ); 
volume_slider.value( foo.gain() );
volume_slider.name( "Gain" );
control_view.addElement( volume_slider );


// display the canvas
control_view.display();


//                        -- functions to control elements--

// this function plays the SinOsc when the button is pressed
function void controlNote()
{
    while( true )
    {
        // wait for the button to be pushed down
        on_button => now; // button event
        <<< "on", "" >>>; // print to console
        on_led.light();  // light up the led       
        .5 => g.gain; // set the main volume as .5 (play the note)
       
        // wait for button to be released
        on_button => now;
        <<< "off", "" >>>;
        on_led.unlight();
        0 => g.gain;

    }
}

// updating foo's frequency when the midi note slider event is detected
function void controlMIDINote()
{
    while( true )
    {
        midi_note_slider => now;
        Std.mtof( midi_note_slider.value() ) => foo.freq;
    }
}

// updating foo's gain when the volume slider event is detected
function void controlGain()
{
    while( true )
    {
        volume_slider => now;
        volume_slider.value() => foo.gain;
        
    }
}

// sporking all the control functions
spork ~ controlNote();
spork ~ controlMIDINote();
spork ~ controlGain();

// a forever loop so that time keeps passing
while( true )
    1::day => now;