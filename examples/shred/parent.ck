// If parent.ck is the top-level shred, then me.parent() will be null.
// Otherwise it will be a parent.
<<< "The top-level shred's ID is:", me.id() >>>;
<<< "The top-level shred's parent is:", me.parent() >>>;

fun void findTheParent() {
    // this will be the same as the top-level shred id
    <<< "The sporked shred's ID is:", me.id() >>>;
    <<< "The sporked shred's parent ID is:", me.parent().id() >>>;
}

spork~ findTheParent();
me.yield(); // yield to let validateParent(...) run
