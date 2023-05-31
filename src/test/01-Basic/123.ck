// test args pass through | 1.5.0.0 (ge) added

// make a SndBuf
SndBuf buf => dac;
// make sure this works
"special:dope" => buf.read;
// this with string add
"special:" + "dope" => buf.read;
// this as pass through
"special:dope" => buf.read => buf.read;

// do some stuff
0 => buf.pos;
1::samp => now;

// if we make it here that's good
<<< "success" >>>;
