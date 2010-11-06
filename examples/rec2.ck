// chuck this with other shreds to record to file
// example> chuck foo.ck bar.ck rec2
// 
// this is just like rec.ck, except "special:auto"
// generates a different file name every time

// pull samples from the dac
dac => gain g => WvOut w => blackhole;

// uncomment this next line to dump to data/ (win32: use '\')
// (but make sure there is a data directory before running)
// "data/chuck-session" => w.autoPrefix;

// this is the output file name
"special:auto" => w.wavFilename;
chout => "writing to file: ";
w.filename => stdout;
// any gain you want for the output
.5 => g.gain;

// infinite time loop...
// ctrl-c will stop it, or modify to desired duration
while( true ) 1::second => now;
