// name: room.ck
// desc: Alvin Lucier's "I Am Sitting in a Room"... in ChucK
// author: Nick Shaheed
// date: Fall 2025 ChucKathon

// we need two LiSa objects: one to record the room, 
// and one to play back the feedback loop. These will
// alternate the act of recording and audio playback.
adc => LiSa loop1 => dac;
adc => LiSa loop2 => dac;

// initialize
0.1::minute => dur loopDur => loop1.duration => loop2.duration;

// ramp-on time
200::ms => loop1.recRamp => loop2.recRamp;

// record first iteration of loop, this is where you speak
<<< "Begin recording your monologue..." >>>;
true => loop1.record;

// don't playback audio for the first iteration of the loop
loop1.duration() => now;
// turn off loop1 recording...
false => loop1.record;

// the feedback loop
while( true )
{
    // wtate 1: loop1 will play back audio and loop2 will record
    false => loop2.play => loop1.record;
    true => loop1.play => loop2.record;
    loopDur => now;
    
    // state 2: loop2 will play back audio and loop1 will record
    false => loop1.play => loop2.record;
    true => loop2.play => loop1.record;
    loopDur => now;
}
