//---------------------------------------------------------------------
// name: event-x-bpm-1.ck
// desc: shows how to
//       1) define a custom type of Event (called MyEvent here)
//          to hold our custom data; create a MyEvent called PULSE
//       2) make a metronome to broadcast PULSE, which will wake
//          up all shreds waiting on PULSE; this metronome will also
//          update the 'measure' and 'tick' numbers in PULSE to
//          indicate where we are in the measure
//       3) define two shreds--one() and two()--whose timing is
//          driven by the metronome to a "drum beat"
//---------------------------------------------------------------------

// sometimes it's helpful to define a custom kind of Event
// so we can associate our own data with it
class MyEvent extends Event
{
    // measure number
    int measure;
    // smallest beat subdivision within a measure (e.g., 16th notes)
    // we will assume the event will be broadcast every tick
    int tick;
}

// make a shared event that any shred can wait on
MyEvent PULSE;
// global bpm variable
100 => global float BPM;
// how many ticks per measure?
16 => float ticksPerMeasure;

// spork timekeeper
spork ~ metronome();
// different functions
spork ~ one() @=> Shred @ shOne;
spork ~ two() @=> Shred @ shTwo;
// a function to change the BPM
spork ~ tempoController();

// keep things running
while( true )
{
    1::second => now;
}

// a thing to change BPM when desired
fun void tempoController()
{
    // a loop to control the BPM
    while( true )
    {
        // every two measures, change the BPM for fun
        if( PULSE.tick == 0 && (PULSE.measure%2 == 0) )
        {
            // randomize BPM lol
            Math.random2f( 60, 120 ) => BPM;
            // print BPM
            <<< "new BPM:", BPM >>>;
        }
        
        // wait on the pulse
        PULSE => now;
    }
}

// metronome (our "conductor")
fun void metronome()
{
    while( true )
    {
        // compute every time in case BPM changes
        1/BPM * minute => dur BEAT;
        // assume 4/4, compute durationm of a tick
        BEAT*4/ticksPerMeasure => dur TICK;
        
        // broadcast to all shreds waiting on PULSE
        PULSE.broadcast();
        
        // increment tick count
        PULSE.tick++;
        // print
        cherr <= "."; cherr.flush();
        // if we reached the end of a measure
        if( PULSE.tick >= ticksPerMeasure )
        {
            // increment measure number
            PULSE.measure++;
            // reset tick count to 0
            0 => PULSE.tick;
            // next line
            cherr <= IO.newline();
        }
    
        // advance time by a tick duration
        TICK => now;
    }
}

// define function
fun void one()
{
    SndBuf buffy => dac;
    "special:dope" => buffy.read;
    0 => buffy.gain;
    
    while( true )
    {
        // wait on the shared event
        PULSE => now;
        // play depending on tick number
        if( PULSE.tick == 0 || PULSE.tick == 8 )
        {
            // LOW DOH
            0 => buffy.pos;
            .5 => buffy.rate;
            .5 => buffy.gain;
        }
        else if( PULSE.tick == 4 || PULSE.tick == 12 )
        {
            0 => buffy.pos;
            1 => buffy.rate;
            .5 => buffy.gain;
        }
    }
}

// define function
fun void two()
{
    SndBuf buffy => dac;
    "special:dope" => buffy.read;
    0 => buffy.gain;
    
    while( true )
    {
        // wait on the shared event
        PULSE => now;
        // in this case play every tick
        0 => buffy.pos;
        // HIGH PITCHED DOH
        2 => buffy.rate;
        .1 => buffy.gain;
    }
}
