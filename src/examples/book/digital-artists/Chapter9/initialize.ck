//  Listing 9.8 Initialize.ck file serves as the master program
// initialize.ck manages classes and files

// add public classes first
Machine.add(me.dir()+"/BPM.ck");     // (1) Adds master BPM class definition

// then add instruments, scores, etc.
Machine.add(me.dir()+"/UseBPM.ck");  // (2) Adds test files UseBPM.ck 
Machine.add(me.dir()+"/UseBPM2.ck"); //     and UseBPM2.ck

// wait a bit, then add some more!
4.0 :: second => now;                // (3) Waits a bit and then adds score file
Machine.add(me.dir()+"/myScore.ck"); 
