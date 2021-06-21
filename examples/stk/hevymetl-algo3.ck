// name: hevymetl-algo3.ck
// desc: demo of STK FM HevyMetl (Algorithm 3)
// 
// name: Perry R. Cook
// date: June 2021, for REPAIRATHON 2021

// patch
HevyMetl h => NRev r => dac;
// reverb mix
0.05 => r.mix;
// set delay
h => Delay d => d => r; 
// quarter note length
0.25::second => dur T;
// delay
4*T => d.max => d.delay;
// feedback gain
0.25 => d.gain;

// time loop
while( true )
{
    // LFO depth
    0.0 => h.lfoDepth;
    
    // play notes
    playNote( 66,2*T );
    playNote( 66,T );
    playNote( 66,T );
    playNote( 67,T );
    playNote( 67,3*T );
    playNote( 64,T );
    
    // from time to time...
    if( maybe*maybe ) playNote(64+2*Math.random2(0,5),2*T);
    else playNote(64,2*T);
    
    playNote(66,5*T);
    
    riff();
}

fun void playNote( int note, dur dure )
{
    Std.mtof(note) => h.freq;
    1 => h.noteOn; 
    0.9*(dure) => now;
    1 => h.noteOff;
    0.1*(dure) => now;
}

fun void riff()
{
    [74,69,66,72,67,62,71,67,62,69] @=> int notes[];
    [1,1,1,2,1,1,2,1,1,5] @=> int durs[];
    for (int i; i < notes.cap(); i++)
    {
        Std.mtof(notes[i]) => h.freq;
        1 => h.noteOn;
        if (i == notes.cap()-1) 0.3 => h.lfoDepth;
        durs[i]*T => now;
    }
    1 => h.noteOff;
}
