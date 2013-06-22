// advance time by 1 second
1::second => now;
// advance time by 100 millisecond
100::ms => now;
// advance time by 1 samp (duration of a sample
1::samp => now;
// advance time by less than 1 samp
.024::samp => now;
