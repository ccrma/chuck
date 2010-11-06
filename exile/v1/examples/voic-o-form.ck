// music for replicants

VoicForm voc=> JCRev r => Echo a => Echo b => Echo c => dac;

220.0 => voc.freq;
0.95 => voc.gain;
.8 => r.gain;
.2 => r.mix;
1000::ms => a.max => b.max => c.max;
750::ms => a.delay => b.delay => c.delay;
.50 => a.mix => b.mix => c.mix;

// shred to modulate the mix
fun void vecho_shred( )
{
    0.0 => float decider;
    0.0 => float mix;
    0.0 => float old;
    0.0 => float inc;
    0 => int n;

    // time loop
    while( true )
    {
        std.rand2f(0.0,1.0) => decider;
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
        std.rand2(2,6)::second => now;
    }
}

// let echo shred go
spork ~ vecho_shred();
0.5 => voc.loudness;
0.01 => voc.vibratoGain;
// our main loop
while( true )
{
    2 * std.rand2( 0,2 ) => int bphon;
    bphon => voc.setPhoneme;
    std.rand2f( 0.6, 0.8 ) => voc.noteOn;

    if( std.randf() > 0.7 )
    { 1000::ms => now; }
    else if( std.randf() > .7 )
    { 500::ms => now; }
    else if( std.randf() > -0.8 )
    { .250::second => now; }
    else
    {
        0 => int i;
        4 * std.rand2( 1, 4 ) => int pick;
        0 => int pick_dir;
        0.0 => float pluck;

	for( ; i < pick; i++ )
        {
	    bphon + 1 * pick_dir => voc.setPhoneme;
            std.rand2f(.4,.6) + (float)i*.035 => pluck;
            pluck + 0.0 * (float)pick_dir => voc.noteOn;
            !pick_dir => pick_dir;
            250::ms => now;
        }
    }

    // pentatonic
    2 * std.rand2( 0, 4 ) => int freq;
    if( freq == 6 ) 7 => freq;
    if( freq == 8 ) 9 => freq;
    std.mtof( (float)( 45 + std.rand2(0,2) * 12 + freq ) ) => voc.freq;
}
