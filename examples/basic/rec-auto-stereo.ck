// chuck this with other shreds to record to file
// example> chuck foo.ck bar.ck rec-auto-stereo.ck

// pull samples from the dac
// WvOut2 -> stereo operation
dac => WvOut2 w => blackhole;

// set the prefix, which will prepended to the filename
// do this if you want the file to appear automatically
// in another directory.  if this isn't set, the file
// should appear in the directory you run chuck from
// with only the date and time.
"chuck-session" => w.autoPrefix;

// this is the output file name
"special:auto" => w.wavFilename;
// optionally specify bit depth
// ("special:auto", IO.INT24) => w.wavFilename;

// print it out
<<<"writing to file: ", w.filename()>>>;

// any gain you want for the output
.5 => w.fileGain;

// temporary workaround to automatically close file on remove-shred
null @=> w;

// infinite time loop...
// ctrl-c will stop it, or modify to desired duration
while( true ) 1::second => now;
