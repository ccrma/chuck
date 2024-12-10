// strange (but valid) edge cases...

// first, not so strange...
// print info about Type
<<< CKDoc.describe( Type ) >>>;
// print info about SinOsc
<<< CKDoc.describe( SinOsc ) >>>;

// next, getting strange...
// a local variable whose name shadows an existing type
SinOsc NRev;
// should print out info about SinOsc
<<< CKDoc.describe( NRev ) >>>;

// really strange (but possible) situation...
// a local Type variable whose name shadows another type
Type JCRev;
// should print "" (and not Type's info)
// since here `JCRev` is an empty Type...
<<< CKDoc.describe( JCRev ) >>>;
