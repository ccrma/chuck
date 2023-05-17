//------------------------------------------------------------------------------
// name: chorus.ck
// desc: basic demo of the Chorus UGen
//
// The Chorus UGen adds a chorus effect to a signal. Chorus refers to an audio
// effect in which multiple sounds occur close together in time, and with
// similar pitch. The slight deviations in pitch and time are generally small
// enough such that the signals are not perceived as being out-of-tune. The
// effect is often described as adding "shimmer", "richness", or "complexity"
// to the timbre. When implemented digitally, the chorus effect is acheived by
// taking a source signal, and mixing it with delayed copies of itself. The
// pitch of these copies is usually modulated using another signal like an LFO.
// In ChucK, you can adjust the depth and frequency of this modulation of pitch,
// as well as the amount of delay and how much of the chorus effect is present
// in the mix.
//
// author: Alex Han
// date: Spring 2023
//------------------------------------------------------------------------------

// set up signal chain
SinOsc s[4]; Chorus chor[4];
// chorus often "shines" best with polyphonic textures!
[62, 65, 69, 72] @=> int notes[];

// connect
for( int i; i < s.size(); i++ )
{
    // patch each voice
    s[i] => chor[i] => dac;

    // sine wave as source signal
    s[i].gain( .2 );
    s[i].freq( Std.mtof(notes[i]) );

    // initializing a light chorus effect
    // (try tweaking these values!)
    chor[i].baseDelay( 10::ms );
    chor[i].modDepth( .4 );
    chor[i].modFreq( 1 );
    chor[i].mix( .2 );
}

// time loop
while( true )
{
    // nothing to do here except advance time
    1::second => now;
}
