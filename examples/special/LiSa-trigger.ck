//--------------------------------------------------------------------
// name: LiSa-trigger.ck
// desc: Live sampling utilities for ChucK
//
// author: Dan Trueman, 2007
//
// based on the S.M.E.L.T. (http://smelt.cs.princeton.edu/) envelope
// follower trigger program will trigger start and stop to one-shot
// LiSa buffers. a sort of triggered delay line, so the delays follow
// the player rather than being static. Success depends on tuning the
// envelope follower closely to the input gains on however you are
// getting audio into ChucK.  Mileage will vary....
//--------------------------------------------------------------------

// patch
adc => Gain g => OnePole p => blackhole;
// square the input, by chucking adc to g a second time
adc => g;
// set g to multiply its inputs
3 => g.op;

// set pole position, influences how closely the envelope follows the input
//   : pole = 0 -> output == input; 
//   : as pole position approaches 1, follower will respond more slowly to input
0.995 => p.pole;
// thresholds
.0001 => float threshold;
.00005 => float releaseThresh;

// duration between successive polling
10::ms => dur pollDur;

// LiSa stuff
adc => LiSa lisa => dac;
lisa.duration(25::second);
lisa.recRamp(20::ms);

// start recording input
lisa.record(1);
// voice variable
-1 => int voice;
// rate
float rate;

// read command line args; set rate
// be nice to have pitch follower determine this, or some kind of table....
Std.atof(me.arg(0)) => rate;
if( rate == 0 ) 1. => rate;
<<< "setting rate to: ", rate >>>;

// events
Event attacks[10];
dur newstarttime, newlen;
time starttime_real;

// infinite time loop
while( true )
{
    // detect onset
    if( p.last() > threshold )
    {
        // do something
        <<< "attack!; starting voice", voice >>>;
        // play last sample
        if( voice > -1 ) attacks[voice].signal();
        lisa.recPos() => newstarttime;
        now => starttime_real;
        
		// wait for release
        while( p.last() > releaseThresh ) { pollDur => now; }
        <<< "release..." >>>;

		// spork off new sample
		now - starttime_real => newlen;
        lisa.getVoice() => voice;
        if( voice > -1 ) spork ~ playlast( attacks[voice], newstarttime, newlen, rate, voice );
    }
    
    // determines poll rate
    pollDur => now;
}

// sporkee
fun void playlast( Event on, dur starttime, dur len, float newrate, int myvoice ) 
{
	if( newrate == 0. ) 1. => newrate;
    <<< "sporking shred with rate: ", newrate >>>;
    if( rate > 0. ) lisa.playPos( myvoice, starttime );
    else lisa.playPos( myvoice, lisa.recPos() - 1::ms );

    // wait    
    on => now;

    lisa.rate( myvoice, newrate );
    lisa.rampUp( myvoice, 20::ms );

    Math.fabs( newrate ) => float absrate;
    len / absrate => now;
    lisa.rampDown( myvoice, ( 250 / absrate )::ms );
    ( 250 / absrate )::ms => now;
    
    // bye bye shred....
}
