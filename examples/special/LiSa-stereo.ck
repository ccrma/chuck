//---------------------------------------------------------------------
// name: LiSa-stereo.ck
// desc: Live sampling utilities for ChucK (stereo edition)
//
// this can readily adapted to more channels, using one of the
// following:
//   ----
//   LiSa2 (stereo)
//   LiSa4 (quad),
//   LiSa6 (6-channel, laptop orchestra edition)
//   LiSa8 (8-channel),
//   LiSa10 (10-channel, for compatibility)
//   LiSa16 (16-channel)
//   ----
// The outputs of these LiSas can be sent to a multichannel dac, or
// simply to other ugens, if it is desirable to process the channels
// in different ways. These multiple channels are available
// regardless of whether the dac is running > 2 chans. LiSaX's
// multi-channel output can be manually connected through .chan(n).
//
// author: Dan Trueman, 2007 (LiSa-munger1.ck)
//         Ge Wang 2023 (LiSa-stereo.ck, based on the above)
//---------------------------------------------------------------------
// patch
SinOsc s => LiSa2 lisa => dac;
// thru
s => dac;

// LiSa duration (this also allocates internals)
lisa.duration( 1::second );
// set max voices
lisa.maxVoices( 30 );
// set voice pan
for( int v; v < lisa.maxVoices(); v++ )
{
    // can pan across all available channels
    // note LiSa.pan( voice, [0...channels-1] )
    lisa.pan( v, Math.random2f( 0, lisa.channels()-1 ) );
}
// set ramp
lisa.recRamp( 20::ms );

// frequency
s.freq( 440 );
// gain
s.gain( 0.25 );

// modulator
SinOsc freqmod => blackhole;
// modulation frequency
freqmod.freq( 0.1 );

// start recording; will loop-record by default
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
