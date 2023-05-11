// MAUI is only available on MAC

// a MAUI demo that uses the "special:dope" sound to demonstrate how you can play
// wav files as both single samples and as looping samples using MAUI
// includes buttons, leds and sliders
// controls the gain and rate of the playback

// sound path
LPF lpf => NRev reverb => dac;

//________________LPF INITIAL NUMS
0 => lpf.Q;
.5 => lpf.gain;

//_____________________________SOUND BUFS________________________________
SndBuf buf_single => lpf;
SndBuf buf_loop => lpf; 

// read in our soundbuf
// sound file
"special:dope.wav" => string buf1;

// read in the soundbuf
buf1 => buf_single.read; 
0 => buf_single.gain; 
buf1 => buf_loop.read; 
0 => buf_loop.gain;


//__________________________GLOBAL VARIABLES
0 => int PLAY_LOOP; // whether the bufs are playing
0 => int LPF_LOOP; // lpf loop toggle
.5 => float SINGLE_GAIN; // play single buf - gain
1. => float SINGLE_RATE; // play single buf - rate
0. => float REVERB;


//__________LOOP CONTROLS___________________

//loop 1 controls
.5 => float LOOP_GAIN; 1. => float LOOP_RATE;


// maui elements
MAUI_View control_view;

// LEDS
MAUI_LED single_led;
MAUI_LED loop_led;
MAUI_LED lpf_loop_led;  

// BUTTONS
MAUI_Button reset;
MAUI_Button single_on; MAUI_Button loop_on;
MAUI_Button lpf_loop;

// SLIDERS
MAUI_Slider singleRate_slider;   MAUI_Slider singleGain_slider;
MAUI_Slider loopRate_slider;      MAUI_Slider loopGain_slider;

MAUI_Slider reverb_mix_slider;


// set view
300 => int CANVAS_X;
400 => int CANVAS_Y;

control_view.size( CANVAS_X, CANVAS_Y); // set the size of the window
control_view.name( "MAUI is Dope" ); // the window name

// BUTTON GLOBAL VARIABLES
150 => int BUTTON_WIDTH;
80 => int BUTTON_HEIGHT;
0 => int BUTTON_Y;
30 => int LED_Y;
100 => int LED_SIZE;
150 => int SLIDER_SIZE;




// ___________SINGLE BUF - ELEMENTS______________

// SINGLE BUTTON
single_on.size( BUTTON_WIDTH, BUTTON_HEIGHT);
single_on.position(0, 0 );
single_on.name( "Single Dope" );
control_view.addElement( single_on);

// PLACE SINGLE LEDs
single_led.size( LED_SIZE, LED_SIZE );
single_led.position(LED_SIZE/4, 35);
single_led.color(single_led.green);
single_led.unlight();
control_view.addElement( single_led );

// PLACE SINGLE SLIDERS
singleRate_slider.range( 0, 3.);
singleRate_slider.size( SLIDER_SIZE, SLIDER_SIZE );
singleRate_slider.position(0, 30);
singleRate_slider.value( 1 );
singleRate_slider.name( "Single Rate" );
control_view.addElement( singleRate_slider );

singleGain_slider.range( 0, 1. );
singleGain_slider.size( SLIDER_SIZE, SLIDER_SIZE );
singleGain_slider.position( 0, 80 );
singleGain_slider.value( .5 );
singleGain_slider.name( "Single Gain" );
control_view.addElement( singleGain_slider );


// ___________LOOP BUF -  ELEMENTS______________

// LOOP BUTTON
loop_on.size( BUTTON_WIDTH, BUTTON_HEIGHT);
loop_on.position(CANVAS_X/2, 0);
loop_on.name( "Loop Dope" );
control_view.addElement( loop_on);

//PLACE LOOP LEDS
loop_led.size( LED_SIZE, LED_SIZE );
loop_led.position(CANVAS_X/2 + LED_SIZE/4, 35);
loop_led.color(loop_led.red);
loop_led.unlight();
control_view.addElement( loop_led );

// PLACE LOOP SLIDERS
loopRate_slider.range( 0, 3. );
loopRate_slider.size( SLIDER_SIZE, SLIDER_SIZE );
loopRate_slider.position( 150, 30 );
loopRate_slider.value( 1 );
loopRate_slider.name( "Loop Rate" );
control_view.addElement( loopRate_slider );

loopGain_slider.range( 0, 1. );
loopGain_slider.size( SLIDER_SIZE, SLIDER_SIZE );
loopGain_slider.position( 150, 80  );
loopGain_slider.value( .5 );
loopGain_slider.name( "Loop Gain" );
control_view.addElement( loopGain_slider );


//__________ REVERB -  UI ELEMENTS_________________________
reverb_mix_slider.range( 0, 1. );
reverb_mix_slider.size( 200, 200 );
reverb_mix_slider.position( CANVAS_X/2 - 100, 100 );
reverb_mix_slider.value( 0 );
reverb_mix_slider.name( "Reverb" );
control_view.addElement( reverb_mix_slider );


//__________ LPF - UI ELEMENTS_________________________

//LPF UI
lpf_loop.size( BUTTON_WIDTH, BUTTON_HEIGHT );
lpf_loop.position( 0, 290  );

lpf_loop.name( "RLPF Sweep Loop" );control_view.addElement(lpf_loop);

lpf_loop_led.size( LED_SIZE, LED_SIZE );
lpf_loop_led.position(CANVAS_X/2 - 1.25*LED_SIZE,325);
lpf_loop_led.color(lpf_loop_led.blue);
lpf_loop_led.unlight();
control_view.addElement( lpf_loop_led );


//______________ RESET - UI ELEMENTS_____________________
reset.size( 100, 80 );
reset.position( CANVAS_X - 100, 320);
reset.name( "Reset" );
control_view.addElement(reset);



// DISPLAY ALL
control_view.display();


//SPORK STUFF
spork ~ ledSingle();   spork ~ ledLoop();

spork ~ ledLpf();
spork ~ reverb_handler();

spork ~ controlSingleGain(); 
spork ~ controlSingleRate(); 

spork ~ controlLoopGain();
spork ~ controlLoopRate();

spork ~ RESET();



//LED/Button Functions
function void ledSingle() 
{
    while( true )
    {
        // wait for the button to be pushed down
        single_on => now;
        single_led.light(); // LED on
        spork ~ play_buf_single();
        // wait for button to be released
        single_on => now; 
        single_led.unlight(); // LED off
    }
}



// LOOP LEDs //

function void ledLoop() // this one has code in for looping buf?
{
    while( true )
    {
        loop_on => now;  // wait for the button to be pushed down
        !PLAY_LOOP => PLAY_LOOP; // set loop to opposite
        if( PLAY_LOOP) {spork ~ play_buf_loop();} // play loop
        spork ~ loop_led.light(); // LED on
        // release toggle
        loop_on => now;
        // turn off
        if (!PLAY_LOOP) spork ~ loop_led.unlight();
        
    }
}


fun void ledLpf()
{
    while( true )
    {
        // wait for the button to be pushed down
        lpf_loop => now;
        !LPF_LOOP => LPF_LOOP;
        spork ~ lpf_loop_led.light();
        // send variable somewhere?
        if( LPF_LOOP) {spork ~ lpf_looper();}
        // wait for button to be released
        lpf_loop => now;
        if (!LPF_LOOP) spork ~lpf_loop_led.unlight();
    }  
}

//________________________ RESONANT LOW PASS FILTER___________________________
fun void lpf_looper() 
{
    .5 => lpf.gain;
    10 => lpf.Q;
    // infinite time-loop
    float t;
    while (LPF_LOOP)
    {
        // sweep the cutoff
        Math.fabs(Math.sin(now/second)) * 5000 => lpf.freq;
        // advance time
        // increment t
        .01 +=> t;
        // advance time
        20::ms => now;
    }
    0 => lpf.Q;
    
}


//_________________________________________________________________

fun void reverb_handler() 
{
    while( true )
    {
        reverb_mix_slider.value() => REVERB;
        //<<< "Reverb" + REVERB >>>;
        REVERB => reverb.mix;
        20::ms => now;
    }
}


//__________________________CONTROL RATE____________________________
function void  controlSingleRate()
{
    while( true )
    {
        singleRate_slider => now; singleRate_slider.value() => SINGLE_RATE;
    }
}

function void  controlLoopRate()
{
    while( true )
    {
        loopRate_slider => now; loopRate_slider.value() => LOOP_RATE;
    }
}



//_________GAIN CONTROL
function void controlSingleGain()
{
    while( true )
    {
        singleGain_slider => now;  singleGain_slider.value()  => SINGLE_GAIN;
    }
}

function void controlLoopGain()
{
    while( true )
    {
        loopGain_slider => now;  loopGain_slider.value()  => LOOP_GAIN;
    }
}


//__________________________________PLAY BUF_________________________________________

// function to control the single one
fun void play_buf_single()
{
    0 => buf_single.pos;
    SINGLE_GAIN => buf_single.gain;
    SINGLE_RATE => buf_single.rate;
    buf_single.length() => now;
}

// function to control the looping one 
fun void play_buf_loop() 
{    
    while(PLAY_LOOP)
    {
        0 => buf_loop.pos;
        LOOP_GAIN => buf_loop.gain;
        LOOP_RATE => buf_loop.rate;
        buf_loop.length() => now;
    }
}

// resets all the values to their original states
fun void RESET()
{
   while (true)
   {
       reset => now;
       <<< "---RESET---", "" >>>;
       0 => PLAY_LOOP; spork ~ loop_led.unlight();
       
       0 =>  LPF_LOOP; spork ~ lpf_loop_led.unlight();
       
       .5 => SINGLE_GAIN; singleGain_slider.value(SINGLE_GAIN); 
       1. => SINGLE_RATE; singleRate_slider.value(SINGLE_RATE); 
       
       .5 => LOOP_GAIN; loopGain_slider.value(LOOP_GAIN); 
       1. => LOOP_RATE; loopRate_slider.value(LOOP_RATE);

       reverb_mix_slider.value(0);
       reset => now;
   }
         
}

1::day => now;

