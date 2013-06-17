// sequencer

// sound chains
SndBuf kick => dac;
SndBuf hihat => dac;
SndBuf snare => dac;

// load sound files with samples
me.dir()  + "/audio/kick_01.wav" => kick.read;
me.dir()  + "/audio/hh_01.wav" => hihat.read;
me.dir()  + "/audio/snare_01.wav" => snare.read;

// set all playheads to end so no sound is made
kick.samples() => kick.pos;
snare.samples() => snare.pos; 
hihat.samples() => hihat.pos;

// initialize counter variable
0 => int counter;

// infinite loop
while( true )
{
    // beat goes from 0 to 7 (8 positions)
    counter % 8 => int beat;
    
    // bass drum 0 and 4
    if ( (beat == 0) || (beat == 4) )
    {
        0 => kick.pos;
    }
    
    // snare on 2 and 6 
    if ( (beat == 2) || (beat == 6) ) 
    {
        0 => snare.pos;
    }
    
    // hi hat play every beat
    0 => hihat.pos;
    .2 => hihat.gain;
    
    <<< "Counter: ", counter, "Beat: ", beat >>>;
    counter++; // counter + 1 => counter; 
    250::ms => now; // advance time
}