// testing LiSa.playPos
// 1.5.5.1 | created by Nick & Ge

// lisa
LiSa l => blackhole;
// 4 sample duration
4::samp => l.duration;
// no record
false => l.record;
// play only
true => l.play;

// loop
true => l.loop0;
for (int i; i < 10; i++) {
    <<< l.playPos() >>>;
    samp => now;
}

// set bi-directional to true
true => l.bi;
for (int i; i < 10; i++) {
    <<< l.playPos() >>>;
    samp => now;
}

// no loop
false => l.loop0;
for (int i; i < 10; i++) {
    <<< l.playPos() >>>;
    samp => now;
}

