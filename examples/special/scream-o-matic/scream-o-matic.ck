//---------------------------------------------------------------------
// name: scream-o-matic.ck
// desc: using multiple LiSas for granular synthesis, to make a
//       continuous scream generator; based on the patch for
//       the augmented FlowBee interface (2014):
//       
//       "Suck-O-Phonic 6000: Augmented Flowbee Musical Interface"
//       https://www.youtube.com/watch?v=g3bkaljCVRs
//
// sound files used in this example:
//       https://chuck.stanford.edu/doc/examples/special/scream-o-matic/data/
//
// author: Alex Chechile
//         Spencer Salazar
//         Ge Wang (https://ccrma.stanford.edu/~ge/)
//---------------------------------------------------------------------
// change this for multi-channel screams
2 => int NUM_CHANNELS;
// per-channel components
NRev reverb[NUM_CHANNELS];
// DC blockers in case any input files has DC/low-freq bias
PoleZero dcblocker[NUM_CHANNELS];

// patch it
for( int i; i < NUM_CHANNELS; i++ )
{
    dcblocker[i] => reverb[i] => dac.chan(i);
    .99 => dcblocker[i].blockZero;
    .1 => reverb[i].mix;
}

// max lisa voices
30 => int LISA_MAX_VOICES;
// grain duration base
250::ms => dur GRAIN_DURATION_BASE;
// grain duration (in multiples of duration base)
6 => float GRAIN_DURATION_FACTOR;
// factor relating grain duration to ramp up/down time
.5 => float GRAIN_RAMP_FACTOR;
// playback rate
1 => float GRAIN_PLAY_RATE;
// grain rate factor (0 means no change)
.2 => float GRAIN_RATE_FACTOR;

// array of screams
LiSa @ lisa[0];
// load data and append to array
lisa << load( me.dir() + "data/scream-1.wav" );
lisa << load( me.dir() + "data/scream-2.wav" );
lisa << load( me.dir() + "data/scream-3.wav" );
lisa << load( me.dir() + "data/scream-4.wav" );
lisa << load( me.dir() + "data/scream-5.wav" );
lisa << load( me.dir() + "data/scream-6-lion.wav" );
// how many to use (remove the `-1` to include lion roar)
lisa.size()-1 => int NUM_LISA;

// connect and pan
for( int i; i < NUM_LISA; i++ )
{
    // connect lisa to audio output
    lisa[i] => dcblocker[i%NUM_CHANNELS];
}

// per-LiSa control shred; the arguments here can be adapted/replaced by
// other input parameters, e.g., from code/kb/mouse/gametrak
fun void go( LiSa @ lisa, float avgR, float varR, float avgT, float varT, dur T )
{
    while( true )
    {
        // fireGrain( LiSa @ lisa, float pos, float rate, dur T, float gain )
        fireGrain( lisa, 
            Math.random2f(.1,.9), Math.random2f(avgR-varR/2,avgR+varR/2), 
            Math.random2f(avgT-varT/2, avgT+varT/2)::ms, 1 );
        T => now;        
    }
}

// spork per-lisa shre
for( int i; i < NUM_LISA; i++ )
{
    // randomize each LiSa for now
    // play with these parameters; maybe randomize differently for different effect
    // go( LiSa @ lisa, float avgR, float varR, float avgT, float varT, dur T )
    spork ~ go( lisa[i], 1, .4, 800, 400, Math.random2f(300,500)::ms );
}

// time loop
while( true )
{ 
    for( int i; i < NUM_LISA; i++ )
    {
        // one of several place to map input (e.g., from kb/mouse/gametrak)
        .5 => lisa[i].gain;
    }
    
    10::ms => now;
}

// fire a grain
fun void fireGrain( LiSa @ lisa, float pos, float rate, dur T, float gain )
{
    // get diff time
    // gt.currTime - gt.lastTime => dur diffTime;
    GRAIN_DURATION_BASE => dur diffTime;
    // grain length
    diffTime * GRAIN_DURATION_FACTOR => dur grainLen;
    // ramp time
    grainLen * GRAIN_RAMP_FACTOR => dur rampTime;
    // a grain
    if( lisa != null && pos >= 0 )
        spork ~ grain( lisa, pos * lisa.duration(), grainLen, T/2, T/2, 
        rate * GRAIN_PLAY_RATE, gain );
}

// sporkee
fun void grain( LiSa @ lisa, dur pos, dur grainLen, dur rampUp, dur rampDown, float rate, float gain )
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
        // voice inga
        lisa.voiceGain( voice, gain );
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