//-----------------------------------------------------------------------------
// name: LiSa-munger1.ck
// desc: Live sampling utilities for ChucK
//
// author: Dan Trueman, 2007
//
// These three example files demonstrate a couple ways to approach granular 
// sampling with ChucK and LiSa. All are modeled after the munger~ from 
// PeRColate. One of the cool things about doing this in ChucK is that there
// is a lot more ready flexibility in designing grain playback patterns;
// rolling one's own idiosyncratic munger is a lot easier. 
//
// Example 1 (below) is simple, but will feature some clicking due to playing
// back over the record-point discontinuity.
//-----------------------------------------------------------------------------

// patch
SinOsc s => LiSa lisa => dac;
// thru
s => dac;

// freq params
s.freq( 440 );
s.gain( 0.2 );

// modulator
SinOsc freqmod => blackhole;
freqmod.freq( 0.1 );

// LiSa params
lisa.duration( 1::second );
// set it recording constantly; loop records by default
lisa.record( 1 );
lisa.gain( 0.1 );

// compute
now + 1000::ms => time later;
while( now < later )
{
    // update   
    freqmod.last() * 500. + 200. => s.freq;
    10::ms => now;
}

// record
lisa.record(0);
s.gain(0);
lisa.recRamp( 20::ms );
lisa.maxVoices(30);
// <<< lisa.maxvoices() >>>;

// this arrangment will create some clicks because of discontinuities
// from the loop recording. to fix, need to make a rotating buffer 
// approach. see the next two example files....
while( true )
{
    // random
    Math.random2f( 1.5, 2.0 ) => float newrate;
    Math.random2f( 250, 750 ) * 1::ms => dur newdur;

    // spork it
    spork ~ getgrain( newdur, 20::ms, 20::ms, newrate );

    // advance time
    10::ms => now;
}

// sporkee
fun void getgrain( dur grainlen, dur rampup, dur rampdown, float rate )
{
    lisa.getVoice() => int newvoice;
    //<<<newvoice>>>;
    
    if( newvoice > -1 )
    {
        lisa.rate(newvoice, rate);
        // lisa.playpos(newvoice, Math.random2f(0., 1000.) * 1::ms);
        lisa.playPos(newvoice, 20::ms);
        // <<< lisa.playpos(newvoice) >>>;
        lisa.rampUp( newvoice, rampup );
        (grainlen - (rampup + rampdown)) => now;
        lisa.rampDown( newvoice, rampdown );
        rampdown => now;
    }
}
