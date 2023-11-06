// return a shred's parent shred (i.e., the shred that sporked it)
// if the shred is a top-level shred, then me.parent() will return null
// related: ancestor.ck

<<< "the top-level shred's ID is:", me.id() >>>;
<<< "the top-level shred's parent is:", me.parent() >>>;

fun void findTheParent()
{
    // this will be the same as the top-level shred id
    <<< "the sporked shred's ID is:", me.id() >>>;
    <<< "the sporked shred's parent ID is:", me.parent().id() >>>;
}

// spork a child shred
spork ~ findTheParent();

// yield to let validateParent(...) run
me.yield();
