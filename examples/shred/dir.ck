// me is currnt shred; print directory of current file (if saved)
<<< me.dir() >>>;
// same as above
<<< me.dir( 0 ) >>>;
// one level up
<<< me.dir( 1 ) >>>;
// can also use negative values (same as positive)
<<< me.dir( -2 ) >>>;
// something absurd (should return top level path)
<<< me.dir( 100 ) >>>;
