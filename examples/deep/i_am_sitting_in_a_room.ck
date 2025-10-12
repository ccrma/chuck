// name: i_am_sitting_in_a_room.ck
// desc: a ChucK implementation of Alvin Lucier's "I Am Sitting in a Room"
// author: Nick Shaheed
// date: Fall 2025 ChucKathon

// We need two LiSa objects: one to record the room, 
// and one to play back the feedback loop. These will
// alternate the act of recording and audio playback.
adc => LiSa loop1 => dac;
adc => LiSa loop2 => dac;

0.1::minute => dur loopDur => loop1.duration => loop2.duration;

// Ramp-on time
200::ms => loop1.recRamp => loop2.recRamp;

// Record first iteration of loop, this is where you speak
<<< "Begin recording your monologue..." >>>;
true => loop1.record;

// don't playback audio for the first iteration of the loop
loop1.duration() => now;
false => loop1.record; // turn off loop1 recording...

// the feedback loop
while (true) {
    // State 1: loop1 will play back audio and loop2 will record
    false => loop2.play => loop1.record;
    true => loop1.play => loop2.record;
    loop1.duration() => now;
    
    // State 2: loop2 will play back audio and loop1 will record
    false => loop1.play => loop2.record;
    true => loop2.play => loop1.record;
    
    loop1.duration() => now;
}
