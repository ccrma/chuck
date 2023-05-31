//--------------------------------------------------------------------
// name: LiSa-munger3.ck
// desc: Live sampling utilities for ChucK
//
// author: Dan Trueman, 2007
//
// These three example files demonstrate a couple ways to approach
// granular sampling with ChucK and LiSa. All are modeled after the
// munger~ from PeRColate. One of the cool things about doing this
// in ChucK is that there is a lot more ready flexibility in
// designing grain playback patterns; rolling one's own idiosyncratic
// munger is a lot easier.
//
// Example 3 (below) uses the same structure as Example 2, but
// replicates the groovy tune munging from the original munger~
// helppatch (with pitch transposition filtering and all!).
//--------------------------------------------------------------------

// oscillator as source
SinOsc s => dac;
s.freq( 440 );
s.gain( 0.25 );

// play the munger song!
Envelope fsmooth => blackhole;
spork ~ playtune(250::ms);
spork ~ smoothtune(20::ms);

// transposition table
[0, 4, 7, -2, 12, 15] @=> int pitchtable[];

// use three buffers to avoid clicks
LiSa lisa[3];
// allocated buffer size -- remains static
1::second => dur bufferlen;
// portion of the buffer size to use -- can vary
0.1::second => dur reclooplen;
// indices for rec and play buffers
0 => int recbuf;
2 => int playbuf;

// go through each LiSa
for( 0 => int i; i < lisa.size(); i++ )
{
    // set LiSa params
    lisa[i].duration(bufferlen);
    lisa[i].loopEndRec(reclooplen);
    lisa[i].maxVoices(30);
    lisa[i].clear();
    lisa[i].gain(0.2);
    // if you want to retain earlier passes through the recording buff when loop recording:
    // lisa[i].feedback(0.5); 
    // ramp at extremes of record buffer while recording
    lisa[i].recRamp(20::ms);
    lisa[i].record( false );
    // connect s to each LiSa    
    s => lisa[i] => dac;
}

// start recording in the recbuf LiSa
lisa[recbuf].record( true );

// create grains, rotate record and play bufs as needed
// shouldn't click as long as the grainlen < bufferlen
while( true )
{
    // will update record and playbufs to use every reclooplen
    now + reclooplen => time later;

    // toss some grains
    while( now < later )
    {
        Math.random2f(0, 6) $ int => int newpitch; // choose a transposition from the table
        Std.mtof(pitchtable[newpitch] + 60)/Std.mtof(60) => float newrate;
        Math.random2f(50, 100) * 1::ms => dur newdur; // create a random duration for the grain

        // spork off a grain!
        spork ~ getgrain( playbuf, newdur, 20::ms, 20::ms, newrate );

        // wait a bit.... then do it again, until we reach reclooplen
        5::ms => now;
    }

    // rotate the record LiSa
    lisa[recbuf++].record( false );
    if( recbuf == lisa.size() ) 0 => recbuf;
    lisa[recbuf].record( true );

    // rotate the play LiSa
    playbuf++;
    if( playbuf == lisa.size() ) 0 => playbuf;
}

// for sporking grains; can do lots of different stuff here -- just one example here
fun void getgrain(int which, dur grainlen, dur rampup, dur rampdown, float rate)
{
    // get an available voice
    lisa[which].getVoice() => int newvoice;
    
    // make sure we got a valid voice
    if( newvoice > -1 )
    {
        // set play rate
        lisa[which].rate(newvoice, rate);
        // set play position
        lisa[which].playPos(newvoice, Math.random2f(0,1) * reclooplen);
        // set ramp up duration
        lisa[which].rampUp(newvoice, rampup);
        // wait for grain length (minus the ramp-up and -down)
        (grainlen - (rampup + rampdown)) => now;
        // set ramp down duration
        lisa[which].rampDown(newvoice, rampdown);
        // for ramp down duration
        rampdown => now;
    }
}

// the munger song lives! thanks to luke dubois....
fun void playtune( dur notelen )
{       
    // notes
    [45, 45, 57, 57, 45, 57, 57, 47, 55, 47, 59, 60, 60, 57, 57, 57] @=> int notes[];
    // note counter
    0 => int notectr;

    // set frequency to first note
    Std.mtof(notes[0]) => fsmooth.value => s.freq;

    // infinite time-loop
    while( true )
    {
        // set frequency target
        Std.mtof(notes[notectr++] + 12) => fsmooth.target;
        // wrap note counter if reach end
        if(notectr == notes.size()) 0 => notectr;
        // wait for note length
        notelen => now;
    }
}

// smooth note to note
fun void smoothtune( dur smoothtime )
{
    // set smooth duration
    fsmooth.duration( smoothtime );
    while( true )
    {
        fsmooth.value() => s.freq;
        1::ms => now;
    }
}
