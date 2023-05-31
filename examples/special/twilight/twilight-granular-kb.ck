//--------------------------------------------------------------------
// name: twilight-granular-kb.ck
// desc: Twilight (2013) granular synthesis instrument redux,
//       keyboard (non-interpolated) edition --
//       adapted from the original Gametrak-based instrument
//
// Twilight (2013) for laptop orchestra:
//       http://slork.stanford.edu/works/twilight/
//       https://www.youtube.com/watch?v=chA-4GRCb-I
//       https://vimeo.com/100624271
//
// author: Ge Wang (ge@ccrma.stanford.edu)
// date: initial instrument: Spring 2013
//       redux version: Summer 2014
//       keyboard version: Winter 2015
//
// usage (loads twilight-source.aiff by default):
//        chuck twilight-granular-kb
// usage (with argument):
//        chuck twilight-granular-kb:SOUND.WAV
//
// keyboard control (also see kb() below):
//       UP/DOWN -- grain position
//       LEFT/DOWN -- grain rate / tuning
//       ','/'.' -- grain size
//       '-'/'=' -- grain position random amount
//--------------------------------------------------------------------

// default filename (can be overwritten via input argument)
"twilight-granular.aiff" => string FILENAME;
// get file name, if one specified as input x0argument
if( me.args() > 0 ) me.arg(0) => FILENAME;

// overall volume
1 => float MAIN_VOLUME;
// grain duration base
50::ms => dur GRAIN_LENGTH;
// factor relating grain duration to ramp up/down time
.5 => float GRAIN_RAMP_FACTOR;
// playback rate
1 => float GRAIN_PLAY_RATE;
// grain position (0 start; 1 end)
0 => float GRAIN_POSITION;
// grain position randomization
.001 => float GRAIN_POSITION_RANDOM;
// grain jitter (0 == periodic fire rate)
1 => float GRAIN_FIRE_RANDOM;

// max lisa voices
30 => int LISA_MAX_VOICES;
// load file into a LiSa (use one LiSa per sound)
load( FILENAME ) @=> LiSa @ lisa;

// patch it
PoleZero blocker => NRev reverb => dac;
// connect
lisa.chan(0) => blocker;

// reverb mix
.05 => reverb.mix;
// pole location to block DC and ultra low frequencies
.99 => blocker.blockZero;


// HID objects
Hid hi;
HidMsg msg;

// which joystick
0 => int device;
// get from command line
if( me.args() ) me.arg(0) => Std.atoi => device;

// open joystick 0, exit on fail
if( !hi.openKeyboard( device ) ) me.exit();
// log
<<< "keyboard '" + hi.name() + "' ready", "" >>>;

// keycodes (for MacOS; may need to change for other systems)
45 => int KEY_DASH;
46 => int KEY_EQUAL;
54 => int KEY_COMMA;
55 => int KEY_PERIOD;
79 => int KEY_RIGHT;
80 => int KEY_LEFT;
81 => int KEY_DOWN;
82 => int KEY_UP;

// spork it
spork ~ print();
spork ~ kb();

// main loop
while( true )
{
    // fire a grain
    fireGrain();
    // amount here naturally controls amount of overlap between grains
    (GRAIN_LENGTH / 2 + Math.random2f(0,GRAIN_FIRE_RANDOM)::ms)/2 => now;
}

// fire!
fun void fireGrain()
{
    // grain length
    GRAIN_LENGTH => dur grainLen;
    // ramp time
    GRAIN_LENGTH * GRAIN_RAMP_FACTOR => dur rampTime;
    // play pos
    GRAIN_POSITION + Math.random2f(0,GRAIN_POSITION_RANDOM) => float pos;
    // a grain
    if( lisa != null && pos >= 0 )
        spork ~ grain( lisa, pos * lisa.duration(), grainLen, rampTime, rampTime, 
        GRAIN_PLAY_RATE );
}

// grain sporkee
fun void grain( LiSa @ lisa, dur pos, dur grainLen, dur rampUp, dur rampDown, float rate )
{
    // get a voice to use
    lisa.getVoice() => int voice;

    // if available
    if( voice > -1 )
    {
        // set rate
        lisa.rate( voice, rate );
        // set playhead
        lisa.playPos( voice, pos );
        // ramp up
        lisa.rampUp( voice, rampUp );
        // wait
        (grainLen - rampUp) => now;
        // ramp down
        lisa.rampDown( voice, rampDown );
        // wait
        rampDown => now;
    }
}

// print
fun void print()
{
    // time loop
    while( true )
    {
        // values
        <<< "pos:", GRAIN_POSITION, "random:", GRAIN_POSITION_RANDOM,
            "rate:", GRAIN_PLAY_RATE, "size:", GRAIN_LENGTH/second >>>;
        // advance time
        100::ms => now;
    }
}

// keyboard
fun void kb()
{
    // infinite event loop
    while( true )
    {
        // wait on HidIn as event
        hi => now;
        
        // messages received
        while( hi.recv( msg ) )
        {
            // button donw
            if( msg.isButtonDown() )
            {
                if( msg.which == KEY_LEFT )
                {
                    .005 -=> GRAIN_PLAY_RATE;
                    if( GRAIN_PLAY_RATE < 0 ) 0 => GRAIN_PLAY_RATE;
                }
                else if( msg.which == KEY_RIGHT )
                {
                    .005 +=> GRAIN_PLAY_RATE;
                    if( GRAIN_PLAY_RATE > 2 ) 2 => GRAIN_PLAY_RATE;
                }
                else if( msg.which == KEY_DOWN )
                {
                    .01 -=> GRAIN_POSITION;
                    if( GRAIN_POSITION < 0 ) 0 => GRAIN_POSITION;
                }
                else if( msg.which == KEY_UP )
                {
                    .01 +=> GRAIN_POSITION;
                    if( GRAIN_POSITION > 1 ) 1 => GRAIN_POSITION;
                }
                else if( msg.which == KEY_COMMA )
                {
                    .95 *=> GRAIN_LENGTH;
                    if( GRAIN_LENGTH < 1::ms ) 1::ms => GRAIN_LENGTH;
                }
                else if( msg.which == KEY_PERIOD )
                {
                    1.05 *=> GRAIN_LENGTH;
                    if( GRAIN_LENGTH > 1::second ) 1::second => GRAIN_LENGTH;
                }
                else if( msg.which == KEY_DASH )
                {
                    .9 *=> GRAIN_POSITION_RANDOM;
                    if( GRAIN_POSITION_RANDOM < .000001 ) .000001 => GRAIN_POSITION_RANDOM;
                }
                else if( msg.which == KEY_EQUAL )
                {
                    1.1 *=> GRAIN_POSITION_RANDOM;
                    if( GRAIN_POSITION_RANDOM > 1 ) 1 => GRAIN_POSITION_RANDOM;
                }
            }
        }
    }
}

// load file into a LiSa
fun LiSa load( string filename )
{
    // sound buffer
    SndBuf buffy;
    // load it
    filename => buffy.read;
    
    // new LiSa
    LiSa lisa;
    // set duration
    buffy.samples()::samp => lisa.duration;
    
    // transfer values from SndBuf to LiSa
    for( 0 => int i; i < buffy.samples(); i++ )
    {
        // args are sample value and sample index
        // (dur must be integral in samples)
        lisa.valueAt( buffy.valueAt(i), i::samp );        
    }
    
    // set LiSa parameters
    lisa.play( false );
    lisa.loop( false );
    lisa.maxVoices( LISA_MAX_VOICES );
    
    return lisa;
}
