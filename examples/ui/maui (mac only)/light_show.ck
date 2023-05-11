// MAUI is only available on MAC

//adapted from the led01.ck
//https://audicle.cs.princeton.edu/miniAudicle/mac/doc/examples/led01.ck
// shows 4 leds that randomly light up with each light making a different sound


// sound patch
//(adding one tone to each light to be played when it's lit)
TriOsc foo_a => dac;
TriOsc foo_b => dac;
TriOsc foo_c => dac;

// light "a" will be playing "C4" when lit, etc.
// set intial gain to 0 so they don't make sounds until lit.
// using STD midi to frequency to convert to Hz.
Std.mtof(60) => foo_a.freq; 0 => foo_a.gain; // "C"
Std.mtof(64) => foo_b.freq; 0 => foo_b.gain; // "E"
Std.mtof(67) => foo_c.freq; 0 => foo_c.gain; // "G"

// global variables for timing variation
100 => int MIN;
2000 => int MAX;
.5 => float ON_GAIN;


// MAUI STUFF
MAUI_LED a, b, c;// initate 3 leds with names a,b,c
MAUI_View view; // initiate canvas

view.size( 150, 50 ); //set size of canvas

a.color( a.red ); // set led 'a' as red
a.size( 50, 50 ); 
a.position( 0, 0 );
a.light();

b.color( b.green ); // green led
b.size( 50, 50 );
b.position( 50, 0 );
b.light();

c.color( c.blue );  // blue led
c.size( 50, 50 );
c.position( 100, 0 );
c.light();

// add leds to canvas
view.addElement( a ); 
view.addElement( b );
view.addElement( c );

// show canvas
view.display(); 


// functions to toggle lights randomly
fun void red_light()
{
    while(true)
    {
        a.light(); // light on
        ON_GAIN => foo_a.gain; // sound on, using global variable gain value
        Math.random2(MIN,MAX)::ms => now; // random time (in ms) to wait
        a.unlight(); // light off
        0 => foo_a.gain; // sound off
        Math.random2(MIN,MAX)::ms => now;    
    }
}

fun void green_light()
{
    while(true)
    {
        b.light(); // light on
        ON_GAIN => foo_b.gain;
        Math.random2(MIN,MAX)::ms => now; // random time (in ms) to wait
        b.unlight(); // light off
        0 => foo_b.gain;
        Math.random2(MIN,MAX)::ms => now;    
    }
}


fun void blue_light()
{
    while(true)
    {
        c.light(); // light on
        ON_GAIN => foo_c.gain;
        Math.random2(MIN,MAX)::ms => now; // random time (in ms) to wait
        c.unlight(); // light off
        0 => foo_c.gain;
        Math.random2(MIN,MAX)::ms => now;    
    }
}


// spork light control functions
spork~red_light();
spork~green_light();
spork~blue_light();

// let a lot of time pass so that everything keeps running
1::day => now;