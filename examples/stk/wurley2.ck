// even more music for replicants

// patch
Wurley voc=> JCRev r => Echo a => Echo b => Echo c => dac;

// initial settings
220.0 => voc.freq;
0.95 => voc.gain;
.8 => r.gain;
.1 => r.mix;
1000::ms => a.max => b.max => c.max;
750::ms => a.delay => b.delay => c.delay;
.50 => a.mix => b.mix => c.mix;

// shred to modulate the mix
fun void vecho_Shred( )
{
    0.0 => float decider;
    0.0 => float mix;
    0.0 => float old;
    0.0 => float inc;
    0 => int n;

    // time loop
    while( true )
    {
        Math.random2f(0.0,1.0) => decider;
        if( decider < .3 ) 0.0 => mix;
        else if( decider < .6 ) .08 => mix;
        else if( decider < .8 ) .5 => mix;
        else .15 => mix;

        // find the increment
        (mix-old)/1000.0 => inc;
        1000 => n;
        while( n-- )
        {
            old + inc => old;
            old => a.mix => b.mix => c.mix;
            1::ms => now;
        }
        mix => old;
        Math.random2(2,6)::second => now;
    }
}

// let echo shred go
spork ~ vecho_Shred();

// scale
[ 0, 3, 7, 8, 11 ] @=> int scale[];

// our main time loop
while( true )
{
    // scale
    scale[Math.random2(0,scale.size()-1)] => int freq;
    Std.mtof( ( 45 + Math.random2(0,1) * 12 + freq ) ) => voc.freq;
    Math.random2f( 0.6, 0.8 ) => voc.noteOn;

    // note: Math.randomf() return values between 0 and 1
    if( Math.randomf() > 0.85 )
    { 1000::ms => now; }
    else if( Math.randomf() > .85 )
    { 500::ms => now; }
    else if( Math.randomf() > .1 )
    { .250::second => now; }
    else
    {
        0 => int i;
        2 * Math.random2( 1, 3 ) => int pick;
        0 => int pick_dir;
        0.0 => float pluck;

	for( ; i < pick; i++ )
        {
            Math.random2f(.4,.6) + i*.035 => pluck;
            pluck + 0.03 * (i * pick_dir) => voc.noteOn;
            !pick_dir => pick_dir;
            250::ms => now;
        }
    }
}
