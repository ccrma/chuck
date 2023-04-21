//--------------------------------------------------------------------
// name: LiSa-munger2.ck
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
// Example 2 (below) demonstrates the rotating buffer approach used
// in the munger~; see after the code for an explanation of this
// approach. presumably someone smarter than me will dream up a more
// elegant technique. 
//--------------------------------------------------------------------

// oscillator as source
// fun scary sounds!!!
SinOsc s => dac;
// set frequency
s.freq( 440 );
// set gain
s.gain( 0.25 );

// modulate
SinOsc freqmod => blackhole;
// modulation frequency
freqmod.freq( 0.1 );

// use three LiSas
LiSa lisa[3];
// buffer length
1::second => dur bufferlen;
0 => int recbuf;
2 => int playbuf;

// LiSa params
for( 0 => int i; i < 3; i++ )
{
    lisa[i].duration( bufferlen );
    lisa[i].maxVoices( 30 );
    lisa[i].clear();
    lisa[i].gain( 0.1 );
    lisa[i].feedback( 0.5 );
    lisa[i].recRamp( 20::ms );
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
    // compute later time
    now + bufferlen => time later;

    // toss some grains
    while( now < later )
    {
        // new rate and duration
        Math.random2f(0.5, 2.5) => float newrate;
        Math.random2f(250, 600)::ms => dur newdur;

        // spork a grain!
        spork ~ getgrain(playbuf, newdur, 20::ms, 20::ms, newrate);
        // freq
        freqmod.last() * 400. + 800. => s.freq;
        // advance time
        10::ms => now;
    }

    // rotate the record LiSa
    lisa[recbuf++].record( false );
    if( recbuf == 3 ) 0 => recbuf;
    lisa[recbuf].record( true );

    // rotate the play LiSa
    playbuf++;
    if( playbuf == 3 ) 0 => playbuf;
}

// sporkee: a grain!
fun void getgrain( int which, dur grainlen, dur rampup, dur rampdown, float rate )
{
    // get an available voice
    lisa[which].getVoice() => int newvoice;

    // make sure we got a valid voice   
    if( newvoice > -1 )
    {
        // set play rate
        lisa[which].rate(newvoice, rate);
        // set play position
        lisa[which].playPos(newvoice, Math.random2f(0,1) * bufferlen);
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

/*--------------------------------------------------------------------
Rotating Buffer Explanation (from the munger~ source code) 

the munger~ (and scrubber~) use a rotating three-buffer scheme for
creating clickless grains.  basically, you don't want to be recording
new data anywhere in the buffer where a grain might be playing. so,
we divide the buffer into three equal parts (B1, B2, B3). at the
outset, let's say:

B1 = the recording buffer part (RB)
B2 = a part where nothing is happening (QB, for Quiet Buffer)
B3 = a part where grains are playing from (PB)

let's say each part is N samples long. after we have recorded N
sample into B1, we rotate the functionality, so now:

B1 = PB
B2 = RB
B3 = QB

why?

as long as the grains are guaranteed to be no longer than N samples
(adjusted for playback rate) we are assured that none of them will
have clicks due to recording discontinuities. we need the Quiet
Buffer because when the rotation happens, there may still be grains
playing in what was the PB and is now the QB.

so, this is a bit cumbersome, but it works. the code here is super
duper ugly -- i was learning to code and working out this scheme on
the fly, and the code reflects that, but it does work.
--------------------------------------------------------------------*/
