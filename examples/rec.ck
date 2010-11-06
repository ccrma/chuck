// chuck this with other shreds to record to file
// example> chuck foo.ck bar.ck rec (see also rec2.ck)

// pull samples from the dac
dac => gain g => WvOut w => blackhole;
// this is the output file name
"foo.wav" => w.wavFilename;
chout => "writing to file: ";
w.filename => stdout;
// any gain you want for the output
.5 => g.gain;

// infinite time loop...
// ctrl-c will stop it, or modify to desired duration
while( true ) 1::second => now;
