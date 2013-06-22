// conversion example

// volume as integer
1 => int volume; 
// MIDI note as float
45.6 => float MIDI_note; 

// convert using the $ sign
(volume) $float => float f_volume; 
(MIDI_note) $int => int i_MIDI_note;

// print out results
<<< f_volume, i_MIDI_note >>>;

