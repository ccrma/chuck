// chuck this with other shreds to record to file
// example> chuck foo.ck bar.ck rec.ck
//          (see also rec-auto-stereo.ck)
//
// arguments: rec:<filename>

// filename
string filename;
// get name
if( me.args() > 0 ) me.arg(0) => filename;
// default
if( filename == "" ) "foo.wav" => filename;

// pull samples from the dac
dac => Gain g => WvOut w => blackhole;
// this is the output file name
filename => w.wavFilename;
// optionally specify bit depth
// (filename, IO.INT24) => w.wavFilename;

// print
<<<"writing to file:", "'" + w.filename() + "'">>>;
// any gain you want for the output
.5 => g.gain;

// temporary workaround to automatically close file on remove-shred
null @=> w;

// infinite time loop...
// ctrl-c will stop it, or modify to desired duration
while( true ) 1::second => now;
