//---------------------------------------------------------------------
// name: LiSa-munger1.ck
// desc: Live sampling utilities for ChucK
//       (also see: LiSa-stereo.ck for stereo version of this)
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
// Example 1 (below) is simple, but will feature some clicking due
// to playing back over the record-point discontinuity.
//---------------------------------------------------------------------
// patch
SinOsc s => LiSa lisa => dac;
// thru
s => dac;

// LiSa duration (this also allocates internals)
lisa.duration( 1::second );
// set max voices
lisa.maxVoices( 30 );
// set ramp duration
lisa.recRamp( 20::ms );

// frequency
s.freq( 440 );
// gain
s.gain( 0.25 );

// modulator
SinOsc freqmod => blackhole;
// modulation frequency
freqmod.freq( 0.1 );

// set it recording constantly; loop records by default
lisa.record( true );
// set gain
lisa.gain( 0.1 );

// compute 1 second from now
now + 1000::ms => time later;
while( now < later )
{
    // update frequency
    freqmod.last() * 500. + 200. => s.freq;
    10::ms => now;
}

// stop recording
lisa.record( false );
// set gain to 0 for SinOsc
s.gain( 0 );

// this arrangment will create some clicks because of discontinuities
// from the loop recording. to fix, need to make a rotating buffer 
// approach. see the next two example files...
while( true )
{
    // random
    Math.random2f( 1.5, 2.0 ) => float newrate;
    Math.random2f( 250, 750 )::ms => dur newdur;

    // spork grain
    spork ~ getgrain( newdur, 20::ms, 20::ms, newrate );

    // advance time
    10::ms => now;
}

// sporkee: entry point for a grain!
fun void getgrain( dur grainlen, dur rampup, dur rampdown, float rate )
{
    // get an available voice
    lisa.getVoice() => int newvoice;

    // make sure we got a valid voice   
    if( newvoice > -1 )
    {
        // set play rate
        lisa.rate(newvoice, rate);
        // set play position
        lisa.playPos(newvoice, 20::ms);
        // lisa.playpos(newvoice, Math.random2f(0., 1000.)::ms);
        // set ramp up duration
        lisa.rampUp( newvoice, rampup );
        // wait for grain length (minus the ramp-up and -down)
        (grainlen - (rampup + rampdown)) => now;
        // set ramp down duration
        lisa.rampDown( newvoice, rampdown );
        // for ramp down duration
        rampdown => now;
    }
}
