//  Listing 9.9 MyScore.ck file adds lots of other BPM users in an endless loop

// myScore.ck, endless loop to add some BPM users
while (true) {
    Machine.add(me.dir()+"/UseBPM2.ck");
    1.0 :: second => now;
    Machine.add(me.dir()+"/UseBPM3.ck");  // (1) New file, UseBPM3.ck
    2.0 :: second => now;
}

