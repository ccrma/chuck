//---------------------------------------------------------------
// name: autotune.ck
// desc: because no one asked for it
//       (and it shows how PitchTrack works)
//
// API reference for PitchTrack
// https://chuck.stanford.edu/doc/reference/chugins.html#PitchTrack
//
// required data file:
// https://chuck.stanford.edu/doc/examples/effects/data/obama.wav
//
// note: PitchTrack is a standard distribution chugin (ChucK plugin)
//       that is included with the macOS and Windows installers;
//       on Linux or if you are building chuck from source, you will
//       need to compile PitchTrack.chug for your system
//       https://github.com/ccrma/chugins/
//---------------------------------------------------------------

// ananlysis
SndBuf obama => PitchTrack tracker => blackhole;
// frame size
512 => tracker.frame;
// frame overlap
4 => tracker.overlap;

// synthesis
obama => HPF highpass => Delay del => PitShift autotune => Envelope fade => dac;
// high pass to get rid of the low din
100 => highpass.freq;

// ramp duration
1::second => fade.duration;
// set up envelope value and target
0 => fade.value;
1 => fade.target;

// input file name
"data/obama.wav" => string filename;
// print
cherr <= "reading input file: '" <= filename <= "'..." <= IO.nl();
// convert to full path relative to this file; read into SndBuf
me.dir() + filename => obama.read;

// modify the '0' to '1' to record a wav file of the output in real-time
// FYI to render faster-than-realtime, run this example with `--silent`
//     > chuck autotune.ck --silent
// FYI this code always records if --silent is enabled
if( 0 || !Machine.realtime() )
{
    dac => WvOut2 record => blackhole;
    // output file name
    "./autotuned-obama.wav" => string outfile;
    // print
    cherr <= "recording to file: '" <= outfile <= "'..." <= IO.nl();
    // set output file name
    me.dir() + outfile => record.wavFilename;
}
else
{
    // print
    cherr <= "recording to file: (disabled)" <= IO.nl();
}

// set delay using pitch tracker frame size
tracker.frame()::samp => del.delay;
// set input file to start
0 => obama.pos;
// length of input file
obama.length() => dur length;

// effect mix
1 => autotune.mix;
// start at 100% (no pitch shift yet)
1 => autotune.shift;

// echoing bells matching vocal pitch
TriOsc tri => ADSR env => Echo e => dac;
// some feedback for recirculating echoes
e => e;
// set oscilator gain
0.15 => tri.gain;
// set ADSR envelope
env.set(5::ms, 250::ms, 0, 0::ms);
// set delay
250::ms => e.max => e.delay;
// set echo
0.5 => e.mix;
// volume
0.8 => e.gain;

// m7 chord
[0, 3, 7, 10] @=> int pentatonic[];

// some variables we'll need later
float last_target;
// compute end time (not to be confused with end times, which is what we are living in)
now + length => time endtime;
// compute when to start fade out
now + (length - fade.duration()) => time fadetime;

// spork parallel processes (can comment out to remove their effect)

// scale switcher (chord progression generator)
spork ~ switchScale();
// play the ostinato pattern
spork ~ ostinato();

// for the duration of the file
while( now < endtime )
{
    // one sample at a time
    samp => now;
    // get pitch estimate
    tracker.get() => float track;

    // find closest pitch
    closest( track, pentatonic ) => float target;

    // only trigger new note if different from previous
    if( target > 60 && Std.fabs(target - last_target) > 0.1 )
    {
        1 => env.keyOn;
        target => tri.freq;
        target => last_target;
    }

    // perform autotune
    if( track > 0 ) target / track => autotune.shift;
    // begin fade out if the time is right
    if( now >= fadetime ) 0 => fade.target;
}

// wait a little extra time
5::second => now;

// hold pitch
fun void hold_pitch( float t )
{
    t => last_target;
    250::ms => now;
    -100 => last_target;
}

// scale switcher (chord progression generator)
fun void switchScale()
{
    while( true )
    {
        12::second => now;
        pentatonic[0]--;
        pentatonic[2]--;
        pentatonic[3]++;
        11.5::second => now;
        pentatonic[0]--;
        2 -=> pentatonic[1];
        pentatonic[2]--;
        3 -=> pentatonic[3];
        for (int i; i<pentatonic.size(); i++)
        {
            if (pentatonic[i] < -5) 12 +=> pentatonic[i];
        }
    }
}

// our repeated pattern
fun void ostinato()
{
    // modal bar into revert
    ModalBar mb => GVerb rev => fade;
    // set gain
    0.5 => mb.gain;
    // which Modal Bar preset
    1 => mb.preset;
    // 0.01 => rev.mix;

    // time loop
    while( true )
    {
        // go through the scale
        for( int degree : pentatonic )
        {
            // pitch to frequency and set in modal bar
            (degree + 48) => Std.mtof => mb.freq;
            // note one!
            1 => mb.noteOn;
            // for an 1/8 of a second
            125::ms => now;
        }
    }
}


// helper function to find equal tempered pitch in list
// which is closest to freq testval
fun float closest( float testval, int list[] )
{
    // list length
    list.size() => int len;

    int octave;
    // freq to midi note number
    Std.ftom(testval) => float testmidi;
    // calculate octave
    while( testmidi - (list[len-1] + octave) > 12 )
    {
        12 +=> octave;
    }

    48000.0 => float lowdiff;
    int closest_index;
    int closest_octave;

    // loop over list
    for( int i; i<len; i++ )
    {
        Std.mtof(octave + list[i]) => float listnote;
        Math.fabs(listnote - testval) => float diff;
        if (diff < lowdiff)
        {
            i => closest_index;
            diff => lowdiff;
            octave => closest_octave;
        }
    }

    // loop over list 8va
    for( int i; i<len; i++ )
    {
        Std.mtof(octave + 12 + list[i]) => float listnote;
        Math.fabs(listnote - testval) => float diff;
        if (diff < lowdiff)
        {
            i => closest_index;
            diff => lowdiff;
            octave + 12 => closest_octave;
        }
    }

    // convert back to freq
    return Std.mtof( closest_octave + list[closest_index] );
}
