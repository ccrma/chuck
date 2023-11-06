// Shred.ancestor() returns the top-level shred that is directly or 
// indirectly a parent shred of the calling shred; useful for getting
// information relevant to top-level shreds; related: parent.ck

// test me
<<< "the top-level shred's ID is:", me.id() >>>;
// my ancestor
<<< "the top-level shred's ancestor ID is:", me.ancestor().id(), "\n" >>>;

// recursive function to test finding ancestor from different 
// "generations" of shreds
fun void findTheAncestor( int generation )
{
    // stop recursing
    if( generation <= 0 ) return;

    // calling shred
    <<< "the sporked shred's ID is:", me.id() >>>;
    // this will always be the same as the top-level shred id
    <<< "the sporked shred's ancestor ID is:", me.ancestor().id(), "\n" >>>;

    // spork a child shred
    spork ~ findTheAncestor( generation-1 );
    // wait a bit to give child shred a chance to run
    1::samp => now;
}

// recursively spork function this 10 times; me.ancestor() should always 
// point to the top-level shred
spork ~ findTheAncestor(10);

// wait to give children shreds a chance to run
1::samp => now;
